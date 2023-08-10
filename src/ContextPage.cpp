#include <tuple>
#include "ContextPage.h"
#include "ChoicePage.h"
#include "AppWizard.h"
#include "FL/Fl_Multiline_Input.H"
#include "FL/Fl_Box.H"
#include <iostream>

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


ContextPage::ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {

    int yGap = 100;
    int xGap = 200;
    //this->testLabel = new Fl_Box(0, 0, this->w, this->h, "frogs");
    testInit(this, w, h);
    this->parent = parent;
    uSpaces = parent->getUSpaces();
    uPrintable = parent->getUPrintable();
    uNewLines = parent->getUNewLines();
    uPrintablePlus = parent->getUPrintablePlus();
    contextDictHere = parent->getContextDict();
    uPdfListHere = parent->getUPdfList();
    newPdfListHere = parent->getNewPdfList();
    uBadCharsHere = parent->getUBadChars();
    uCharOccursHere = parent->getUCharOccurs();
    bindexHere = parent->getBindex();
    newPdfTextHere = parent->getNewPdfText();

    cIndex = 0;
    currBadChar = getCurrBadChar();
    getContexts(currBadChar);


    std::string displayChar = parent->getDisplayChar();
    std::string contextStr = "Current Context: ";

    x += 150;
    // display the current context using Fl_Box
    Fl_Box* contextDisplay = new Fl_Box(x + 200, y, 90, 40, displayChar.c_str());
    y += yGap;
    Fl_Input* replacementInput = new Fl_Input(x, y, 200, 40, "Replacement: ");
    x += xGap;
    Fl_Button* saveBtn = new Fl_Button(x, y, 200, 40, "Submit Replacement");
    x += xGap;
    Fl_Button* noneBtn = new Fl_Button(x, y, 200, 40, "Don't Replace");
    x += xGap;
    Fl_Button* undoBtn = new Fl_Button(x, y, 200, 40, "Undo Choice");
}

// get the current character
UChar32 ContextPage::getCurrBadChar() {
    UChar32 badChar = (*uBadCharsHere)[*bindexHere];
    return badChar;
}


void ContextPage::testInit(ContextPage* thisPage, int w, int h){
    //this->testLabel->copy_label("Hello Motto");
    this->testLabel = new Fl_Box(0, 0, this->w, this->h, "frogs");

}
/*
void ContextPage::newInit(){
    for(auto box : contextBoxes) box->redraw();
    for(auto input : replacementInputs) input->redraw();
    for(auto btn : saveBtns) btn->redraw();
    for(auto btn : noneBtns) btn->redraw();
    for(auto btn : undoBtns) btn->redraw();
}
*/

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
std::pair<int32_t,int32_t> ContextPage::getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength){
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

void ContextPage::getContexts(int thisChar) {
    // 1. for every page in the book 
    for(const auto& page : (*uPdfListHere)){
        // a. for every character in the page
        for(UChar32 thisChar; thisChar < page.length(); thisChar++){
            // i. if the current character is a bad character
            if (page.indexOf(thisChar) != -1) {
                // 1. get the context of the bad character here
                icu::UnicodeString context = getConText(thisChar, page);

                // 2. add this context to the list of contexts if it's not there already
                if ((*contextDictHere)[thisChar].find(context) == (*contextDictHere)[thisChar].end()) {
                    // 2a. (we add this character as its own replacement as a default // placeholder)
                    (*contextDictHere)[thisChar][context] = thisChar;
                    // also add it to the vector of contexts
                    (*contextList).push_back(context);
                }
            }
        }
    }
}
// memory-access issues across pages mean that we have to reconstruct all of these functions here
icu::UnicodeString ContextPage::getConText(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    // error handling
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return icu::UnicodeString("");
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
    // increment the bad character index
    cIndex++;

    // std::cout << "bindexHere Next Char: " << *bindexHere << std::endl;

    // if the bad character index is greater than the number of bad characters
    if(cIndex >= contexts.size()) {
        std::cout << "Going for replacements!" << std::endl;
        // start replacing the bad characters
    } else {
        // otherwise refresh the context values
        refreshContext();
    }
}

void ContextPage::refreshContext() {
    // get the current context, convert it to a string, and display it
    icu::UnicodeString currContext = getCurrentContext();
    std::string currStdContext;
    currContext.toUTF8String(currStdContext);
    std::string newText = "Current Context: " + currStdContext;
    std::cout << "new text: " << newText << std::endl;


    int listSize = listOfContexts.size();
    int boxSize = chartextBoxes.size();
    int listBoxRange = listSize - boxSize;

    // update contexts display
    for (int i = 0; i < listOfContexts.size(); i++) {
        std::cout << "copying label at index " << i << std::endl;
        std::cout << "label: " << listOfContexts[i].c_str() << std::endl;
        chartextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
    // if there's an empty box, fill it with N/A
    if (listSize < boxSize) {
        for (int i = listSize; i < boxSize; i++) {
            chartextBoxes[i]->copy_label("N/A");
        }
    }
}

icu::UnicodeString ContextPage::getCurrentContext(){
    return (*contextDictHere)[thisChar];
}