#include "OpenPDFPage.h"
#include "AppWizard.h"

OpenPDFPage::OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // set parent
    this->parent = parent;

    // buttons to load, output to display bad characters, and button to go to next page
    this->loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
    this->nextBtn = new Fl_Button(250, 50, 150, 40, "Next");
    this->badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");

    // initialise attributes
    this->initAttributes();

    // end construction
    this->end();
}

// function to grab attributes from parent...make the code less of an eyesore
void OpenPDFPage::initAttributes(){
    // get uSpaces from parent
    this->uSpaces = this->parent->getUSpaces();

    // get uPrintable from parent
    this->uPrintable = this->parent->getUPrintable();

    // get uNewLines from parent
    this->uNewLines = this->parent->getUNewLines();

    // get uPrintable from parent
    this->uPrintablePlus = this->parent->getUPrintablePlus();

    // get the choicePage from parent
    this->choicePageHere = this->parent->getChoicePage();

    // pointers to initially extracted text
    this->uPdfTextHere = this->parent->getUPdfText();
    this->uPdfListHere = this->parent->getUPdfList();

    // pointers to whitespace & new-line cleaned text
    this->newPdfTextHere = this->parent->getNewPdfText();
    this->newPdfListHere = this->parent->getNewPdfList();

    // link to bad characters and their occurrences
    this->uBadChars = this->parent->getUBadChars(); 
    this->uCharOccurs = this->parent->getUCharOccurs();

    // activate buttons
    this->loadBtn->callback(activateLoad, this);
    this->nextBtn->callback(goToChoicePage, choicePageHere);

}

void OpenPDFPage::activateLoad(Fl_Widget* w, void* data) {
    OpenPDFPage* instance = static_cast<OpenPDFPage*>(data);
    instance->loadPDFDoc();
}

// function to load up a pdf document
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
        for (int pageDex = 0; pageDex < numPages; pageDex++) {
            // create a unique pointer to the page
            std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(pageDex)); 

            // if that created unique pointer is valid
            if (pageIt) {
                poppler::byte_array pageBytes = pageIt->text().to_utf8(); // get data in utf8 encoding
                std::string strPageText(pageBytes.begin(), pageBytes.end()); // convert bytes to string
                icu::UnicodeString uPageText(strPageText.c_str(), "UTF-8"); // convert string to unicode string

                // push the page text to the pageList vector
                this->uPdfListHere->push_back(uPageText); // push to the big page string
                this->uPdfTextHere->append(uPageText); // push to the pageList vector
            } 
            
            pageIt.reset(); // reset the page pointer (i'm traumatized by memory leaks)
        }
    } else {
        return ; // if the PDF is invalid, return
    }
    // if every thing went well, process the PDF
    this->processPDFDoc();

}

void OpenPDFPage::processPDFDoc() {
    // for each page in the PDF, run processing function
    for (const auto& uPdfPage : *uPdfListHere) {
        // initialise a new page in our empty newPdfList    
        this->newPdfListHere->push_back(icu::UnicodeString(""));
        // process the already-existing page
        this->processPageText(uPdfPage);
    }
    // build & display output message
    this->makeOutput(this->badOut);
}

void OpenPDFPage::processPageText(const icu::UnicodeString& uPageText) {
    // get the length of the page
    int32_t thisLen = uPageText.length();
    
    // assume leading whitespaces
    bool leadingWhiteSpace = true;

    // for every character in the page
    for (int32_t charIt = 0; charIt < thisLen; ) {
        // get the character
        UChar32 uChar = uPageText.char32At(charIt);

        // process the character
        processChar(uChar, leadingWhiteSpace, charIt);

        // increment the character iterator
        charIt += U16_LENGTH(uChar);
    }
}

// function that determines what type of character a character is, thence what to do with it
void OpenPDFPage::processChar(UChar32 uChar, bool& leadingWhiteSpace, int32_t charIt) {
    // if character is a space
    if(leadingWhiteSpace && this->uSpaces->find(uChar) != this->uSpaces->end()){
        // just skip
        return;
    } 
    
    // else, if the character is a newline-like
    else if(this->uNewLines->find(uChar) != uNewLines->end()){
        // simply add a newline to the string
        *(this->newPdfTextHere) += icu::UnicodeString("\n", "UTF-8");
        this->newPdfListHere->back() += icu::UnicodeString("\n", "UTF-8");
        leadingWhiteSpace = true;  
    } 

    // else, if the character is a "good" one
    else if (this->uPrintable->find(uChar) != uPrintable->end()) {
        // no more leading whitespace
        leadingWhiteSpace = false;
        *(this->newPdfTextHere) += uChar; // add it to the big string
        this->newPdfListHere->back() += uChar; // add it to big list

    } 
    // otherwise, it has to be a "bad" one
    else {
        // no more leading whitespace
        leadingWhiteSpace = false;

        // if we haven't see this bad character yet, add it our list of bad characters
        if(this->uCharOccurs->find(uChar) == this->uCharOccurs->end()){
            *(this->uBadChars) += uChar;
        }

        (*(this->uCharOccurs))[uChar] += 1; // increment the number of occurrences of this bad character
        *(this->newPdfTextHere) += uChar; // add it to the big string
        this->newPdfListHere->back() += uChar; // add it to big list

    }
}

// function to build the output message
void OpenPDFPage::makeOutput(Fl_Multiline_Output* badHere) {
    // initialise output message
    std::string outputMessage = std::to_string(uCharOccurs->size()) + " bad characters found:\n";

    // *shudders*
    if(this->uCharOccurs) {
        // go thru every key-value pair in the dictionary
        for(const auto& [thisChar, occur] : *(uCharOccurs)) {
            // wrap the char in unicode string then convert that to std::string
            std::string charStr;
            icu::UnicodeString(thisChar).toUTF8String(charStr);
            
            // append character to output message
            outputMessage += charStr + ": " + std::to_string(occur) + "\n";
        }
    } else {
        outputMessage += "No bad characters detected.\n";
    }

    // send the constructed message to the Fl_Multiline_Output widget
    this->badOut->value(outputMessage.c_str());
}

void OpenPDFPage::goToChoicePage(Fl_Widget* widget, void* data) {
    // unpack choice page from data
    ChoicePage** choicePageHere = static_cast<ChoicePage**>(data);

    // tell the wizard to go to the next page
    // widget = button; parent1 = page; parent2 = wizard; it's ugly but it works
    ((Fl_Wizard*)widget->parent()->parent())->next(); 
    (**choicePageHere).refreshVals(); // initialise the next page

}