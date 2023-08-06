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
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Output.H>

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

// function to load pdfs
void loadPDF(Fl_Widget* w, void* data) {
    // open a file chooser dialog
    const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL);

    // if it's a valid file
    if (thisPDF) {
        // print the filename to the terminal
        std::cout << "Selected PDF: " << thisPDF << std::endl;

        // load PDF with poppler
        std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF));

        // if the loaded PDF is valid, then parse it and extract text
        if (popplerDoc) {
            // get the total number of pages
            int numPages = popplerDoc->pages();

            // for every page in the PDF
            for (int page_dex = 0; page_dex < numPages; ++page_dex) {
                // initialise a pointer to the beginning of this page
                std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex));

                // if the pointer is valid
                if (pageIt) {
                    // extract text data from poppler in UTF-8 encoding
                    poppler::byte_array byteArray = pageIt->text().to_utf8();
                    // convert to std::string
                    std::string text(byteArray.begin(), byteArray.end());
                    // push to our string-list of pages
                    pageList.push_back(text);
                }
            }
        } 

        // open .txt file to write to disk (we send hex stuff so it's in UTF-8 encoding)
        std::ofstream outFile("pageOut.txt");
        outFile << char(0xEF) << char(0xBB) << char(0xBF);

        std::ofstream badtxt("badChars.txt", std::ios::binary);
        badtxt << char(0xEF) << char(0xBB) << char(0xBF); 
        badtxt << "begin bad characters" << '\n' << ' '; 

        // print every non-printable character in the vector
        // for every page
        for(int pageIt = 0; pageIt < pageList.size(); ++pageIt) {
            // for every character in the page
            bool leadingWhiteSpace = true; // to skip leading whitespace
            for(int charIt = 0; charIt < pageList[pageIt].size(); ++charIt) {
                char thisChar = pageList[pageIt][charIt];
                // if character is a newline, reset leading whitespace 
                if(leadingWhiteSpace && thisChar == ' ') {
                    continue;
                } else if(thisChar == '\n'){
                    leadingWhiteSpace = true;
                    outFile << thisChar;

                } else {
                    leadingWhiteSpace = false;
                    if(printable.find(thisChar) == std::string::npos) {
                        // send it to the bad character list if it's not there already
                        if(badChars.find(thisChar) == std::string::npos) {
                            badChars += thisChar;
                            badtxt << thisChar;
                            badtxt << ' ' << '\n' << ' ';
                        }
                    // add to number of character occurrences
                    charOccur[thisChar]++;
                    outFile << thisChar;
                    } else {
                        // add to number of character occurrences
                        charOccur[thisChar]++;
                        outFile << thisChar;
                    }
                } 
            }
        }

        // close .txt files for pages and bad characters
        outFile.close();
        badtxt.close(); 

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

int main(int argc, char** argv) {
    // create container window
    Fl_Window* win = new Fl_Window(1000, 800, "PDF Scrivener");

    // button that loads the PDF
    Fl_Button* btn = new Fl_Button(50, 50, 150, 40, "Load PDF");

    // multiline output for bad characters
    Fl_Multiline_Output* out = new Fl_Multiline_Output(50, 200, 900, 600, "");

    // make the button do stuff
    btn->callback(loadPDF, out);  

    // no more widgets
    win->end();
    // show everything
    win->show(argc, argv);

    // start
    return Fl::run();
}
