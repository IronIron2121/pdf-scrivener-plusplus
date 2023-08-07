#include "OpenPDFPage.h"
#include "AppWizard.h"

// hahahahahahahahahahahahahahahahahahahahahahahahahahahahahahha
std::string* OpenPDFPage::badChars;
std::string OpenPDFPage::printable;
std::string OpenPDFPage::pageStr;
std::vector<std::string> OpenPDFPage::pageList;
std::unordered_map<char, int>* OpenPDFPage::charOccur;

OpenPDFPage::OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // button to load, output to display bad characters, and button to go to next page
    loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
    nextBtn = new Fl_Button(250, 50, 150, 40, "Next");
    badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");

    printable = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n";
    pageStr = "";
    badChars = parent->getBadChars(); // yeahhh I mean we could do pointers for this too but we don't have to so i'm not gonna
    std::string pageList = "";
    charOccur = parent->getCharOccur();

    if(!charOccur) {
        std::cout << "charOccur is null" << std::endl;
    } else{
        std::cout << "charOccur is not null" << std::endl;
    }

    PassData* passData = new PassData;
    passData->badOut = badOut;
    passData->parent = parent;
    
    // activate buttons
    loadBtn->callback(loadPDF, passData);
    nextBtn->callback(goToChoicePage);

    end();
}

void OpenPDFPage::loadPDF(Fl_Widget* w, void* data) {
    // grab the stuff we wrapped up for the callback. static cast to contextualise void...
    PassData* carePackage = static_cast<PassData*>(data);
    // unpack it into the output and parent 
    Fl_Multiline_Output* badHere = carePackage->badOut;
    AppWizard* parentHere = carePackage->parent;
    
    const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL); // open file chooser 

    // if it's a valid file
    if (thisPDF) {
        // try to load it as a PDF
        std::cout << "Selected PDF: " << thisPDF << std::endl;
        std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF)); // load the PDF

        // if the loaded PDF is valid, then parse it and extract text
        if (popplerDoc) {
            int numPages = (popplerDoc->pages()); // get the number of pages in the PDF
            // for every page in the PDF
            for (int page_dex = 0; page_dex < numPages; page_dex++) {
                std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex)); // create a unique pointer to the page

                // if the pointer is valid
                if (pageIt) {
                    poppler::byte_array byteArray = pageIt->text().to_utf8(); // get data in utf8 encoding
                    std::string text(byteArray.begin(), byteArray.end()); // convert bytes to string
                    pageList.push_back(text); // push to the pageList vector
                }
                pageIt.reset(); // reset the page pointer
            }
            // for every page
            for(int pageInt = 0; pageInt < pageList.size(); pageInt++) {
                bool leadingWhiteSpace = true; // to skip leading whitespaces

                // for every character in the page
                for(int charIt = 0; charIt < pageList[pageInt].size(); charIt++) {
                    // initialise a string for the page
                    std::string pageText = "";
                    // grab the character
                    char thisChar = pageList[pageInt][charIt]; 


                    // if character is a newline, reset leading whitespace 
                    if(leadingWhiteSpace && thisChar == ' ') {
                        continue; // if it's a leading whitespace, skip it
                    } else if(thisChar == '\n'){
                        // if it's a newline, reset leading whitespace
                        leadingWhiteSpace = true;
                        // add it to the book string and the page string
                        pageStr += thisChar; 
                        pageText += thisChar;


                    } else {
                        // otherwise, it's not a leading whitespace
                        leadingWhiteSpace = false;
                        // we add it to the book string whether it's good or bad
                        pageStr += thisChar; 
                        pageText += thisChar;

                        // if it's not a printable (good) character
                        if(printable.find(thisChar) == std::string::npos) {
                            // if it's not already been added to the bad character list
                            if(badChars->find(thisChar) == std::string::npos) {
                                *badChars += thisChar; // add to bad character list
                            }
                            parentHere->upCharOccur(thisChar);// add to number of this character's occurrences
                        } else {
                            // if it's a good character, just add to the number of this character's occurrences
                            parentHere->upCharOccur(thisChar);
                        }
                    }  
                }
                
            }
            // create ostream to announce number of bad characters
            std::ostringstream os;
            os << "This document has " << badChars->size() << " bad characters:\n\n";

            // loop thru the charOccur map, append each bad character and its num occurrences
            for(int i = 0; i < badChars->size(); ++i) {
                os << i << ' ' << (*badChars)[i] << ", with: " << (*charOccur)[(*badChars)[i]] << " occurrences\n";
            }


            // send bad character notification to the output
            badHere->value(os.str().c_str());
            badHere->redraw(); // redraw the output
            std::cout << os.str() << std::endl; // print to console
            std::cout << "Processing complete" << std::endl;
        } else{
            std::cout << "Invalid PDF" << std::endl;
        }
    } else {
        std::cout << "No PDF selected" << std::endl;
    }
}

void OpenPDFPage::goToChoicePage(Fl_Widget* w, void* data) {
    // tell the wizard to go to the next page
    ((Fl_Wizard*)w->parent()->parent())->next();
}