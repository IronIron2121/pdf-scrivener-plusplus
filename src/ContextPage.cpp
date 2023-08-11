#include <tuple>
#include "ContextPage.h"
#include "ChoicePage.h"
#include "AppWizard.h"
#include "FL/Fl_Multiline_Input.H"
#include "FL/Fl_Box.H"
#include <iostream>

ContextPage::ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // init parent
    this->parent = parent;
    // init pointers
    this->initPointers();
    // init attributes
    this->initAttributes();
    // init display
    this->initDisplays();
    this->end();
}

void ContextPage::initPointers(){
    this->bIndexHere = this->parent->getBIndex();
    this->cIndexHere = this->parent->getCIndex();
    this->uBadCharsHere = this->parent->getUBadChars();
    this->contextDictHere = this->parent->getContextDict();
    this->newPdfListHere = this->parent->getNewPdfList();
    this->contextListHere = this->parent->getContextList();
}

void ContextPage::initAttributes(){
    this->yGap = 100;
    this->xGap = 200;
    this->listSize = 0;
}

void ContextPage::initDisplays(){
    this->x += this->xGap;
    // display the current context using Fl_Box
    this->contextDisplay = new Fl_Box(this->x + 200, this->y, 90, 60, "init");
    this->y += this->yGap;
    this->replacementInput = new Fl_Input(this->x, this->y, 200, 40, "Replacement: ");
    this->x += this->xGap;
    this->saveBtn = new Fl_Button(this->x, this->y, 200, 40, "Submit Replacement");
    this->x += this->xGap;
    this->noneBtn = new Fl_Button(this->x, this->y, 200, 40, "Don't Replace");

    this->saveBtn->callback(saveBtnCb, this);
    this->noneBtn->callback(noneBtnCb, this);
}

void ContextPage::saveBtnCb(Fl_Widget* widget, void* data) {
    ContextPage* page = static_cast<ContextPage*>(data);

    // get current value in the text box
    std::string replacement = page->replacementInput->value();
    // convert it to a unicode string
    icu::UnicodeString replacementU(replacement.c_str());
    // save replacement in the context dictionary
    (*(page->contextDictHere))[page->parent->getCurrBadChar()][page->parent->getCurrentContext()] = replacementU;

    // convert dictionary index to string
    std::string testStr;
    (*(page->contextDictHere))[page->parent->getCurrBadChar()][page->parent->getCurrentContext()].toUTF8String(testStr);


    std::cout << "REPLACED " << page->parent->getCurrBadChar() << " WITH " << testStr << std::endl;
    // Move to the next context
    page->nextContext();
}

void ContextPage::noneBtnCb(Fl_Widget* widget, void* data) {
    ContextPage* page = static_cast<ContextPage*>(data);
    // just move to the next context - self-replacement is the default
    page->nextContext();
}



void ContextPage::newInit(){
    // get the contexts for this character and refresh page
    this->parent->getContextsForRep(this->parent->getCurrBadChar());
    this->listSize = (*(this->contextListHere))[this->parent->getCurrBadChar()].size();
    this->refreshContext();

}

void ContextPage::nextContext() {
    // increment the context index
    this->parent->upCIndex();

    // if the bad character index is greater than the number of bad characters
    if(*cIndexHere >= (*(this->contextListHere))[this->parent->getCurrBadChar()].size()) {
        std::cout << "\nContext index: " << *(this->cIndexHere) << std::endl;
        std::cout << "Context Lists Size: " << ((*(this->contextListHere))[this->parent->getCurrBadChar()].size()) << std::endl;
        this->hide();
        this->parent->choicePage->nextChar();
        this->parent->choicePage->show();
    } else {
        // otherwise refresh the context values
        this->refreshContext();
    }
}

void ContextPage::refreshContext() {
    // get the current context, convert it to a string, and display it
    icu::UnicodeString currContext = this->parent->getCurrentContext(); // get context
    std::string currStdContext; // init empty string
    currContext.toUTF8String(currStdContext); // dump context to string
    std::string newText = "Context " + std::to_string((*(this->parent->getCIndex())) + 1) + " of " + std::to_string(((*(this->parent->getContextList()))[this->parent->getCurrBadChar()]).size()) + ": " + currStdContext; // build display string

    this->contextDisplay->copy_label(newText.c_str()); // change the display label

}
    /*
    INIT
    retrieve_badchar()
    retrieve_contexts()
    display_context(i)
    display_buttons(i)
    IF user presses "submit replacement"
        IF i < contexts.size()
            save replacement
            i++
            display_context(i)
        ELSE
            save replacement
            hide_this_page()
            display_choice_page()
    
    this should circumvent the problem of redrawing the page
    */
