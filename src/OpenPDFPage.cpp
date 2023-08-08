#include "OpenPDFPage.h"
#include "AppWizard.h"

// hahahahahahahahahahahahahahahahahahahahahahahahahahahahahahha
/*
std::vector<std::string>* OpenPDFPage::badChars;
std::string* OpenPDFPage::pdfStr;
std::string OpenPDFPage::accounted;
std::string OpenPDFPage::printable;
std::string OpenPDFPage::printablePlus;

std::vector<std::string> OpenPDFPage::pageList;
std::unordered_map<std::string, int>* OpenPDFPage::charOccur;
*/

OpenPDFPage::OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // button to load, output to display bad characters, and button to go to next page
    loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
    nextBtn = new Fl_Button(250, 50, 150, 40, "Next");
    badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");

    uAccounted = icu::UnicodeString::fromUTF8(" \f\t\v\r\n.,:;!?()[]{}<>\"'`");
    uPrintable = icu::UnicodeString::fromUTF8(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n");
    uPrintablePlus = uPrintable + uAccounted;
    uPageList = {};
    uPdfStr = parent->getPdfText();
    uBadChars = parent->getBadChars(); 
    uCharOccur = parent->getCharOccur();

    if(!uCharOccur) {
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
                    } else{
                        if(thisChar == '\n'){
                            *pdfStr += thisChar;
                            pageText += thisChar;
                            // if it's a newline, reset leading whitespace
                            leadingWhiteSpace = true;
                        } else {
                            // if we're here, it's a non-leading whitepace character
                            leadingWhiteSpace = false;

                            // if it's not a printable (good or accounted for) character
                            if(printablePlus.find(thisChar) == std::string::npos) {
                                // find the rightmost limit of this context
                                int contIt = charIt;
                                while(contIt < pageList[pageInt].size() && printablePlus.find(pageList[pageInt][contIt]) == std::string::npos) {
                                    contIt++;
                                }
                                // extract a slice around the bad character
                                std::string slice = pageList[pageInt].substr(charIt, contIt);
                                // convert the slice to a unicode string
                                icu::UnicodeString uStr(slice.c_str(), slice.size(), "UTF-8"); 
                                // get the first (i.e. bad) character in the unicode string
                                UChar32 uChar = uStr.char32At(0);

                                // convert the unicode char back to a string...
                                // extract a UTF8 substring of length (this bad character) from the unicode string
                                // ...I know, I'm exhausted too
                                std::string backToStr;
                                uStr.tempSubString(0, U16_LENGTH(uChar)).toUTF8String(backToStr);

                                // add it to the pdfStr
                                *pdfStr += backToStr;
                                pageText += backToStr;

                                // if it's not already been added to the bad character list
                                if(std::find(badChars->begin(), badChars->end(), backToStr) == badChars->end()) {
                                    badChars->push_back(backToStr); // add to bad character list
                                }
                                // iterate forwards, past the unicode character
                                charIt += U16_LENGTH(uChar);
                                parentHere->upCharOccur(backToStr);// add to number of this character's occurrences
                            } else {
                                *pdfStr += thisChar;
                                pageText += thisChar;
                                std::string thisCharStr(1, thisChar);
                                // if it's a good character, just add to the number of this character's occurrences
                                parentHere->upCharOccur(thisCharStr);
                            }
                            parentHere->pushToPdfPages(pageText);
                        }
                    }  
                }
                
            }
            // create ostream to announce number of bad characters
            std::ostringstream os;
            os << "This document has " << badChars->size() << " bad characters:\n\n";
            // print ascii value of first bad character 
            std::cout << "ASCII value of the first bad character: " << static_cast<int>((*badChars)[1].front()) << std::endl;

            // loop thru the charOccur map, append each bad character and its num occurrences
            for(int i = 0; i < badChars->size(); ++i) {
                os << i << ' ' << (*badChars)[i] << ", with: " << (*charOccur)[(*badChars)[i]] << " occurrences\n";
            }

            // send bad character notification to the output
            badHere->value(os.str().c_str());
            badHere->redraw(); // redraw the output
            std::cout << os.str() << std::endl; // print to console
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
    // refresh the values of the choice page
    //((AppWizard*)data)->refreshVals(data);
}