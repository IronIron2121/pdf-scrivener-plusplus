#include <tuple>
#include "ContextPage.h"
#include "ChoicePage.h"
#include "AppWizard.h"
#include "FL/Fl_Multiline_Input.H"
#include "FL/Fl_Box.H"
#include <iostream>

ContextPage::ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {

    int yGap = 100;
    int xGap = 200;
    //this->testLabel = new Fl_Box(0, 0, this->w, this->h, "frogs");
    bIndexHere = parent->getBIndex();
    uBadCharsHere = parent->getUBadChars();
    contextDictHere = parent->getContextDict();
    newPdfListHere = parent->getNewPdfList();
    uCharOccursHere = parent->getUCharOccurs();
    contextListHere = parent->getContextList();
    cIndexHere = parent->getCIndex();
    this->parent = parent;


    currBadChar = getCurrBadChar();


    std::string displayChar = parent->getDisplayChar();
    std::string contextStr = "Current Context: ";

    x += 150;
    // display the current context using Fl_Box
    Fl_Box* contextDisplay = new Fl_Box(x + 200, y, 90, 40, "init");
    y += yGap;
    Fl_Input* replacementInput = new Fl_Input(x, y, 200, 40, "Replacement: ");
    x += xGap;
    Fl_Button* saveBtn = new Fl_Button(x, y, 200, 40, "Submit Replacement");
    x += xGap;
    Fl_Button* noneBtn = new Fl_Button(x, y, 200, 40, "Don't Replace");

    saveBtn->callback(saveBtnCb);
    noneBtn->callback(noneBtnCb);
}

void ContextPage::saveBtnCb(Fl_Widget* widget, void* data) {
    ContextPage* page = static_cast<ContextPage*>(data);

    // get current value in the text box
    std::string replacement = page->replacementInput->value();
    // convert it to a unicode string
    icu::UnicodeString replacementU(replacement.c_str());
    // save replacement in the context dictionary
    (*page->contextDictHere)[page->getCurrBadChar()][page->getCurrentContext()] = replacementU;
    // Move to the next context
    page->nextContext();
}

void ContextPage::noneBtnCb(Fl_Widget* widget, void* data) {
    ContextPage* page = static_cast<ContextPage*>(data);
    // just move to the next context
    page->nextContext();
}



// get the current character
UChar32 ContextPage::getCurrBadChar() {
    UChar32 badChar = (*uBadCharsHere)[*bIndexHere];
    return badChar;
}


void ContextPage::newInit(AppWizard** thisParent){
    //this->testLabel->copy_label("Hello Motto");
    // get the current character as uchar
    bIndexHere = (*thisParent)->getBIndex();
    uBadCharsHere = (*thisParent)->getUBadChars();
    contextDictHere = (*thisParent)->getContextDict();
    newPdfListHere = (*thisParent)->getNewPdfList();
    uCharOccursHere = (*thisParent)->getUCharOccurs();
    contextListHere = (*thisParent)->getContextList();
    cIndexHere = (*thisParent)->getCIndex();
    std::cout << "PARENT ADDRESS IN AND CONTEXT == " << &(*thisParent) << std::endl;
    std::cout << "BINDEX ADDRESS IN CONTEXT == " << &bIndexHere << std::endl;


    UChar32 nowChar = (*uBadCharsHere)[0];

    // turn that into unicode
    icu::UnicodeString nowCharU(nowChar);

    // turn that into a string
    std::string nowCharStr;
    nowCharU.toUTF8String(nowCharStr);

    std::cout << "NEW INIT AT CHARACTER: " << nowCharStr << ", BINDEX = " << (*bIndexHere) << std::endl;

    getContexts((*uBadCharsHere)[*bIndexHere]);
    refreshContext();
}


void ContextPage::deleteThis(std::map<icu::UnicodeString, ContextWidgets> contextWidgetsMap){
    for (const auto& pair : contextWidgetsMap) {
        delete pair.second.contextBox;
        delete pair.second.replacementInput;
        delete pair.second.saveBtn;
        delete pair.second.noneBtn;
        delete pair.second.undoBtn;
    }
    contextWidgetsMap.clear();
}

bool ContextPage::endChecker(UChar32 thisChar, const icu::UnicodeString& enders) {
    // if this character is an ender, return true
    return enders.indexOf(thisChar) != -1;
}
std::pair<int32_t,int32_t> ContextPage::getPointers(int32_t indx, const icu::UnicodeString& pageText, const int32_t thisPageLength){
    // characters that end a sentence
    icu::UnicodeString enders = u" .,\n";
    // init context span indices
    int32_t leftPointer = indx;
    int32_t rightPointer = indx;
    // find the leftmost limit of the context
    while(!endChecker(pageText.charAt(leftPointer-1), enders) && leftPointer > 0) {
        leftPointer--;
    }
    // find the rightmost limit of the context
    while(!endChecker(pageText[rightPointer], enders) && rightPointer < thisPageLength-1) {
        rightPointer++;
    }

    // make them into a pair and return
    std::pair<int32_t,int32_t> this_out = std::make_pair(leftPointer, rightPointer);
    return this_out;
}

void ContextPage::getContexts(UChar32 thisChar) {
    std::cout << "GETTING CONTEXTS AT CONTEXT PAGE" << std::endl;
    //int i = 0;
    // 1. for every page in the book
    for(const auto page : (*newPdfListHere)){
        //i++;
        //std::cout << "AT PAGE " << i << std::endl;
        // a. for every character in the page
        int32_t pageIt = 0;
        while(pageIt < page.length()){
            // i. if the current character is our current bad character
            UChar32 pageChar = page.charAt(pageIt);
            if (pageChar == thisChar) {
                // 1. get the context of the bad character here
                icu::UnicodeString context = getConText(pageIt, page);

                // 2. add this context to the list of contexts if it's not there already
                if ((*contextDictHere)[thisChar].find(context) == (*contextDictHere)[thisChar].end()) {
                    // 2a. (we add this character as its own replacement as a default // placeholder)
                    (*contextDictHere)[thisChar][context] = thisChar;
                    // also add it to the vector of contexts
                    (*contextListHere)[(*uBadCharsHere)[(*cIndexHere)]].push_back(context);
                    // print from this index to test if it worked
                    icu::UnicodeString test = (*contextListHere)[(*uBadCharsHere)[(*cIndexHere)]][*cIndexHere];
                    std::string testStr;
                    test.toUTF8String(testStr);
                    std::cout << "TEST STRING: " << testStr << std::endl;
                }
            }
            pageIt += U16_LENGTH(pageChar);
        }
    }
}

// memory-access issues across pages mean that we have to reconstruct all of these functions here
icu::UnicodeString ContextPage::getConText(int32_t indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    // error handling
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return icu::UnicodeString("");
    } else{
        std::cout << "All Clear!!" << std::endl;
    }

    // get the pointers for this run and assign them
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;

    // get the relative position of the bad character
    int32_t leftDiff = indx - leftPointer;

    // construct a string that tells users which char in a string they're meant to be looking at
    icu::UnicodeString posNotif = "Pos: ";
    icu::UnicodeString strNumber = icu::UnicodeString::fromUTF8(std::to_string(leftDiff + 1));
    posNotif += strNumber;
    posNotif += " ";

    // extract the context 
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);

    return context;
}

void ContextPage::nextContext() {
    // increment the context index
    (*bIndexHere)++;
    (*cIndexHere)++;

    // if the bad character index is greater than the number of bad characters
    if((*cIndexHere) >= contexts.size()) {
        std::cout << "Going back to choice page!!" << std::endl;
        this->hide();
        return;

    } else {
        // otherwise refresh the context values
        refreshContext();
    }
}




void ContextPage::refreshContext() {
    // get the size of the list of contexts for this character
    int listSize = (*contextListHere)[(*uBadCharsHere)[*bIndexHere]].size();
    // if we've passed the end of the list of contexts, return to the choice page
    if ((*cIndexHere) >= listSize) {
        this->hide();
    }

    // get the current context, convert it to a string, and display it
    icu::UnicodeString currContext = getCurrentContext(); // get context
    std::string currStdContext; // init empty string
    currContext.toUTF8String(currStdContext); // dump context to string
    std::cout << "Current Context: " << currStdContext << std::endl; // print context
    std::string newText = "Current Context: " + currStdContext; // build display string
    
    Fl_Box* thisContextDisplay = getContextDisplay(); // get the display
    thisContextDisplay->copy_label(newText.c_str()); // change the display label
}

icu::UnicodeString ContextPage::getCurrentContext(){
    return (*contextListHere)[getCurrBadChar()][*cIndexHere];
}

Fl_Box* ContextPage::getContextDisplay() {
    return contextDisplay;
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
