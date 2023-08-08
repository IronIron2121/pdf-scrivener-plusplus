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
    uPdfText = parent->getUPdfText();
    uPdfList = parent->getUPdfList();

    uBadChars = parent->getUBadChars(); 
    uCharOccurs = parent->getUCharOccurs();

    if(!uCharOccurs) {
        std::cout << "charOccur is null" << std::endl;
    } else{
        std::cout << "charOccur is not null" << std::endl;
    }

    // activate buttons
    loadBtn->callback(activateLoad, this);
    nextBtn->callback(goToChoicePage);

    end();
}

static void activateLoad(Fl_Widget* w, void* data) {
    OpenPDFPage* instance = static_cast<OpenPDFPage*>(data);
    instance->loadPDFDoc();
}

void OpenPDFPage::loadPDFDoc(){
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
                parent->pushToUPdfList(uPageText); // push to the pageList vector
                parent->pushToUPdfText(uPageText); // push to the pageList vector
            } 
            
            pageIt.reset(); // reset the page pointer
        }
    }
}

void OpenPDFPage::processPDFDoc() {
    // for each page in the PDF, run processing function
    for (const auto& uPdfPage : *uPdfList) {
        processPageText(uPdfPage);
    }

    // build & display output message
    //makeOutput();
}

void OpenPDFPage::processPageText(const icu::UnicodeString& uPageText) {
    // account for potential leading whitespaces
    bool leadingWhiteSpace = true; 

    // get the length of the page
    int32_t len = uPageText.length();

    // Using a UChar iterator for UnicodeString
    for (int32_t charIt = 0; charIt < len; charIt++) {
        UChar32 uChar = uPageText.char32At(charIt);
        processChar(uChar, leadingWhiteSpace);
    }
}

void OpenPDFPage::processChar(UChar32 uChar, bool leadingWhiteSpace) {
    // if character is a space
    if(leadingWhiteSpace && uSpaces.indexOf(uChar) != -1){
        // just skip
    } else if (uPrintablePlus.indexOf(uChar) != -1) {
        // if it's a good character, just add it to the occurrences map
        *uPdfText += uChar;
    } else {
        // if it's not, add it to the bad character list AND add it to the 
        parent->upUCharOccur(uChar);
        *uPdfText += uChar;
    }
    // finish this off, lololol
}


void OpenPDFPage::makeOutput(Fl_Multiline_Output* badHere) {
    // add code here to make and display output message
}
// --------------------
// --------------------
// --------------------

void OpenPDFPage::goToChoicePage(Fl_Widget* w, void* data) {
    // tell the wizard to go to the next page
    ((Fl_Wizard*)w->parent()->parent())->next();
    // refresh the values of the choice page
    //((AppWizard*)data)->refreshVals(data);
}