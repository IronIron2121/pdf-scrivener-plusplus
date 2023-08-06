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
class MyPage : public Fl_Group {
public:
    MyPage(int x, int y, int w, int h, const char* title = 0) : Fl_Group(x, y, w, h, title) {}
};

// window that lets you choose what to do with the bad characters
class ChoicePage : public MyPage {
private:
    Fl_Box* heyHowUDoin;

public:
    ChoicePage(int x, int y, int w, int h, const char* title = 0) : MyPage(x, y, w, h, title) {
        heyHowUDoin = new Fl_Box(450, 350, 100, 100, "You look fine as a newly minted dime"); // Centered HelloWorld box
        end();
    }
};


class OpenPDFPage : public MyPage {
private:
    Fl_Button* loadBtn;
    Fl_Multiline_Output* badOut;
    Fl_Button* nextBtn;

public:
    OpenPDFPage(int x, int y, int w, int h, const char* title = 0) : MyPage(x, y, w, h, title) {
        loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
        badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");
        nextBtn = new Fl_Button(250, 50, 150, 40, "Next");

        loadBtn->callback(loadPDF, badOut);
        nextBtn->callback(goToChoicePage);  

        end();  
    }

    // load PDF, extract text, and find bad characters
    static void loadPDF(Fl_Widget* w, void* data) {
        const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL); // open file chooser 
        // if it's a valid file
        if (thisPDF) {
            // try to load it as a PDF
            std::cout << "Selected PDF: " << thisPDF << std::endl;
            std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF)); // load the PDF


            // if the loaded PDF is valid, then parse it and extract text
            if (popplerDoc) {
                std::cout << "Processing PDF" << std::endl;
                int numPages = (popplerDoc->pages()); // get the number of pages in the PDF
                std::cout << "Number of pages: " << numPages << std::endl;
                // for every page in the PDF
                for (int page_dex = 0; page_dex < numPages; page_dex++) {
                    std::cout << "Processing page " << page_dex << std::endl;
                    std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex)); // create a unique pointer to the page

                    // if the pointer is valid
                    if (pageIt) {
                        poppler::byte_array byteArray = pageIt->text().to_utf8(); // get data in utf8 encoding
                        std::string text(byteArray.begin(), byteArray.end()); // convert bytes to string
                        pageList.push_back(text); // push to the pageList vector
                    }
                    pageIt.reset(); // reset the page pointer


                }
                std::cout << "Finished Processing PDF" << std::endl;
                // for every page
                for(int pageInt = 0; pageInt < pageList.size(); pageInt++) {
                    std::cout << "Processing page " << pageInt << std::endl;
                    bool leadingWhiteSpace = true; // to skip leading whitespaces

                    // for every character in the page
                    for(int charIt = 0; charIt < pageList[pageInt].size(); charIt++) {
                        // grab the character
                        char thisChar = pageList[pageInt][charIt]; 

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
                Fl_Multiline_Output* badOut = (Fl_Multiline_Output*)data;

                // send bad character notification to the output
                badOut->value(os.str().c_str());
                badOut->redraw(); // redraw the output
                std::cout << os.str() << std::endl; // print to console
                std::cout << "Processing complete" << std::endl;
            } else{
                std::cout << "Invalid PDF" << std::endl;
            }
        } else {
            std::cout << "No PDF selected" << std::endl;
        }

        std::cout << "Finished Loading PDF" << std::endl;
    }
    static void goToChoicePage(Fl_Widget* w, void* data) {
        // tell the wizard to go to the next page
        ((Fl_Wizard*)w->parent()->parent())->next();
    }
};

class AppWizard : public Fl_Window {
private:
    Fl_Wizard* wizard;
    OpenPDFPage* openPage;
    ChoicePage* choicePage;

public:
    AppWizard(int w, int h, const char* title = 0) : Fl_Window(w, h, title){
        wizard = new Fl_Wizard(0, 0, w, h);
        openPage = new OpenPDFPage(0, 0, w, h, "PDF Scrivener - Open PDF");
        choicePage = new ChoicePage(0, 0, w, h, "PDF Scrivener - Choice Page");

        // add the pages to the wizard
        wizard->add(openPage);
        wizard->add(choicePage);

        // hide all pages except the first
        wizard->value(openPage);

        wizard->end();
    }
};


int main(int argc, char** argv) {
    AppWizard appWin(1000, 800, "PDF Scrivener");
    appWin.show(argc, argv);
    return Fl::run();
}
