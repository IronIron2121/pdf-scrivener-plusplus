#include "OpenPDFPage.h"
#include "AppWizard.h"

OpenPDFPage::OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // button to load, output to display bad characters, and button to go to next page
    loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
    nextBtn = new Fl_Button(250, 50, 150, 40, "Next");
    badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");

    // get uSpaces from parent
    uSpaces = parent->getUSpaces();
    // get uPrintable from parent
    uPrintable = parent->getUPrintable();
    // get uNewLines from parent
    uNewLines = parent->getUNewLines();
    // get uPrintable from parent
    uPrintablePlus = parent->getUPrintablePlus();

 
    // pointers to various things we need to access by reference
    uPdfTextHere = parent->getUPdfText();
    uPdfListHere = parent->getUPdfList();

    newPdfTextHere = parent->getNewPdfText();
    newPdfListHere = parent->getNewPdfList();

    uBadChars = parent->getUBadChars(); 
    uCharOccurs = parent->getUCharOccurs();

    if(!uCharOccurs) {
        std::cout << "charOccur is null" << std::endl;
    } else{
        std::cout << "charOccur is not null" << std::endl;
    }

    // activate buttons
    loadBtn->callback(activateLoad, this);
    nextBtn->callback(goToChoicePage, parent);

    end();
}

void OpenPDFPage::activateLoad(Fl_Widget* w, void* data) {
    OpenPDFPage* instance = static_cast<OpenPDFPage*>(data);
    instance->loadPDFDoc();
}

void OpenPDFPage::loadPDFDoc(){
    std::vector<icu::UnicodeString> testVec;
    // open file chooser 
    const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL); 

    // if it's a valid file
    if (!thisPDF) {
        std::cout << "No PDF selected" << std::endl;
    }

    // try to load it as a PDF
    std::cout << "Selected PDF: " << thisPDF << std::endl;
    std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF)); // load the PDF

    // if the loaded PDF is valid, then parse it and extract text
    if (popplerDoc) {
        int numPages = (popplerDoc->pages()); // get the number of pages in the PDF
        
        // for every page in the PDF
        for (int page_dex = 0; page_dex < numPages; page_dex++) {
            // create a unique pointer to the page
            std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex)); 

            // if the pointer is valid
            if (pageIt) {
                poppler::byte_array pageBytes = pageIt->text().to_utf8(); // get data in utf8 encoding
                std::string strPageText(pageBytes.begin(), pageBytes.end()); // convert bytes to string
                icu::UnicodeString uPageText(strPageText.c_str(), "UTF-8"); // convert string to unicode string

                // push the page text to the pageList vector
                uPdfListHere->push_back(uPageText);
                uPdfTextHere->append(uPageText); // push to the pageList vector
            } 
            
            pageIt.reset(); // reset the page pointer
        }
    } else {
        return ; // if the PDF is invalid, return
    }
    // if every thing went well, process the PDF
    processPDFDoc();

}

void OpenPDFPage::processPDFDoc() {
    // for each page in the PDF, run processing function
    for (const auto& uPdfPage : *uPdfListHere) {
        newPdfListHere->push_back(icu::UnicodeString(""));
        processPageText(uPdfPage);
    }
    // build & display output message
    makeOutput(badOut);
}

void OpenPDFPage::processPageText(const icu::UnicodeString& uPageText) {
    // get the length of the page
    int32_t thisLen = uPageText.length();
    // assume leading whitespaces
    bool leadingWhiteSpace = true;
    // add a page to the newPdfList

    // UChar iterator for unicode string
    for (int32_t charIt = 0; charIt < thisLen; charIt++) {
        UChar32 uChar = uPageText.char32At(charIt);
        processChar(uChar, leadingWhiteSpace, charIt);
    }
}
void OpenPDFPage::processChar(UChar32 uChar, bool& leadingWhiteSpace, int32_t charIt) {
    // if character is a space
    if(leadingWhiteSpace && (*uSpaces).find(uChar) != (*uSpaces).end()){
        // just skip
        return;
    } else if((*uNewLines).find(uChar) != (*uNewLines).end()){
        // if it's a newline-like, just add a new line
        *newPdfTextHere += icu::UnicodeString("\n", "UTF-8");
        (*newPdfListHere).back() += icu::UnicodeString("\n", "UTF-8");
        leadingWhiteSpace = true;
        
    } else if ((*uPrintable).find(uChar) != (*uPrintable).end()) {
        // if it's a good character, just add it to the string
        leadingWhiteSpace = false;
        *newPdfTextHere += uChar;
        (*newPdfListHere).back() += uChar;

    } else {
        // if it's a bad character, add to its occurrences, add it to the string, and the list
        leadingWhiteSpace = false;
        // if we haven't see it yet, add it to the list of bad characters
        if((*uCharOccurs).find(uChar) == (*uCharOccurs).end()){
            (*uBadChars) += uChar;
        }

        (*uCharOccurs)[uChar] += 1;
        *newPdfTextHere += uChar;
        (*newPdfListHere).back() += uChar;

    }
}


void OpenPDFPage::makeOutput(Fl_Multiline_Output* badHere) {
    // start output message
    std::string outputMessage = std::to_string(uCharOccurs->size()) + " bad characters found:\n";

    // error handling 
    if(uCharOccurs) {
        // go thru every key-value pair in the dictionary
        for(const auto& [thisChar, occur] : *uCharOccurs) {
            // wrap the char in unicode string then convert to std::string
            std::string charStr;
            icu::UnicodeString(thisChar).toUTF8String(charStr);
            
            // append character to output message
            outputMessage += charStr + ": " + std::to_string(occur) + "\n";
        }
    } else {
        outputMessage += "No bad characters detected.\n";
    }

    // Set the constructed message to the Fl_Multiline_Output widget
    badHere->value(outputMessage.c_str());
}

// --------------------
// --------------------
// --------------------

void OpenPDFPage::goToChoicePage(Fl_Widget* w, void* data) {
    // tell the wizard to go to the next page
    // there's gotta be a cleaner way of doing this
    ((Fl_Wizard*)w->parent()->parent())->next();

}