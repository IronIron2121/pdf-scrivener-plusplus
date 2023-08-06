// builtins
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <random>
#include <sstream>
#include <unordered_map>

// FLTK for GUI stuff
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Wizard.H>

// poppler for pdf parsing
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>


// vector to contain extracted text from pdf
std::vector<std::string> pageList;
std::vector<std::string> convList; // after converting for display purposes

// the pdf we use for testing and filepath for directory
std::string pdfInputs = "bin/inputs/";
std::string pdfFileName = "GoodHeart";
std::string pdfFilePath = pdfInputs + pdfFileName + ".pdf";

// list of acceptable characters
std::string printable = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n";

// character occurrences
std::unordered_map<char, int> charOccur;

// empty list of unacceptable characters
std::string badChars = "";

// generic page class. I should probably put this in a header file
class Page : public Fl_Window {
public:
    Page(int w, int h, const char* title = 0) : Fl_Window(w, h, title) {}
};

// window that lets you choose what to do with the bad characters
class ChoicePage : public Page {
private:
    Fl_Box* heyHowUDoin;

public:
    ChoicePage(int w, int h, const char* title = 0) : Page(w, h, title) {
        heyHowUDoin = new Fl_Box(450, 350, 100, 100, "You look fine as a newly minted dime"); // Centered HelloWorld box
        end();
    }
};


class OpenPDFPage : public Page {
private:
    Fl_Button* btn;
    Fl_Multiline_Output* out;
    Fl_Button* nextBtn;

public:
    OpenPDFPage(int w, int h, const char* title = 0) : Page(w, h, title) {
        btn = new Fl_Button(50, 50, 150, 40, "Open PDF");
        out = new Fl_Multiline_Output(50, 200, 900, 600, "");    
        btn->callback(loadPDF, out);  

        nextBtn = new Fl_Button(250, 50, 150, 40, "Next");
        nextBtn->callback(goToChoicePage);  

        end();  
    }

    // load PDF, extract text, and find bad characters
    static void loadPDF(Fl_Widget* w, void* data) {
        const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL); // open file chooser 
        // if it's a valid file
        if (thisPDF) {
            std::cout << "Selected PDF: " << thisPDF << std::endl; // print the filepath to terminal
            std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF)); // load the PDF
            
            // if the loaded PDF is valid, then parse it and extract text
            if (popplerDoc) {
                int numPages = popplerDoc->pages(); // get the number of pages in the PDF

                // for every page in the PDF
                for (int page_dex = 0; page_dex < numPages; ++page_dex) {
                    std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex)); // init a pointer to the page beginning

                    // if the pointer is valid
                    if (pageIt) {
                        poppler::byte_array byteArray = pageIt->text().to_utf8(); // get data in utf8 encoding
                        std::string text(byteArray.begin(), byteArray.end()); // convert bytes to string
                        pageList.push_back(text); // push to the pageList vector
                    } 
                }
            } 

            std::ofstream outFile("pageOut.txt"); // open a .txt file to write text to (DEBUGGING)
            outFile << char(0xEF) << char(0xBB) << char(0xBF); // assert utf8 encoding

            // same old story
            std::ofstream badtxt("badChars.txt", std::ios::binary);
            badtxt << char(0xEF) << char(0xBB) << char(0xBF); 

            // for every page
            for(int pageIt = 0; pageIt < pageList.size(); ++pageIt) {
                bool leadingWhiteSpace = true; // to skip leading whitespaces

                // for every character in the page
                for(int charIt = 0; charIt < pageList[pageIt].size(); ++charIt) {
                    char thisChar = pageList[pageIt][charIt]; // grab the character

                    // if character is a newline, reset leading whitespace 
                    if(leadingWhiteSpace && thisChar == ' ') {
                        continue; // if it's a leading whitespace, skip it
                    } else if(thisChar == '\n'){
                        // if it's a newline, reset leading whitespace
                        leadingWhiteSpace = true;

                    } else {
                        // otherwise, it's not a leading whitespace
                        leadingWhiteSpace = false;

                        // if it's not a printable (good) character
                        if(printable.find(thisChar) == std::string::npos) {
                            if(badChars.find(thisChar) == std::string::npos) {
                                badChars += thisChar; // add to bad character list
                            }
                        charOccur[thisChar]++; // add to number of character occurrences
                        } else {
                            // otherwise, it's a printable character. do the same thing as above
                            charOccur[thisChar]++;
                        }
                    } 
                }
            }

            // create ostream to announce number of bad characters
            std::ostringstream os;
            os << "This document has " << badChars.size() << " bad characters:\n\n";

            // loop thru the charOccur map, append each bad character and its num occurrences
            for(int i = 0; i < badChars.size(); ++i) {
                os << i << ' ' << badChars[i] << ", with: " << charOccur[badChars[i]] << " occurrences\n";
            }

            // container for multiline output
            Fl_Multiline_Output* out = (Fl_Multiline_Output*)data;

            // send bad character notification to the output
            out->value(os.str().c_str());
            out->redraw(); // redraw the output

            std::cout << "Processing complete" << std::endl;

        } else {
            std::cout << "No PDF selected" << std::endl;
        }
    }    

    static void goToChoicePage(Fl_Widget* w, void* data) {
        ((Fl_Window*)w->window())->hide(); // hide this window
        ChoicePage choiceWin(1000, 800, "PDF Scrivener - Choice Page"); 
        choiceWin.show(); // show the next window
        Fl::run(); // run the next window
    }
};


int main(int argc, char** argv) {
    OpenPDFPage openPDFWin(1000, 800, "PDF Scrivener - Open PDF");
    openPDFWin.show(argc, argv);
    return Fl::run();
}