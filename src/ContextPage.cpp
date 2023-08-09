#include <tuple>
#include "ContextPage.h"
#include "ChoicePage.h"
#include "AppWizard.h"
#include "FL/Fl_Multiline_Input.H"
#include <iostream>


ContextPage::ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    //getContexts(thisChar);
    this->y = y;
    this->x = x;
    this->w = w;
    this->h = h;
    this->xGap = 100;
    this->yGap = 100;
}

void ContextPage::newInit(){
    // localise the attributes   
    initAttributes(); 
    int x = this->x;
    int y = this->y;
    int xGap = this->xGap;
    int yGap = this->yGap;
    
    for (const auto& context : contexts) {

        // padding
        y = y + yGap;

        // convert the current context to std::string
        std::string contextStr;
        context.toUTF8String(contextStr);

        // display the current context using Fl_Box
        Fl_Box* contextDisplay = new Fl_Box(x + 200, y, 90, 40, contextStr.c_str());
        Fl_Input* replacementInput = new Fl_Input(x + 300, y, 150, 40, "Replacement: ");
        Fl_Button* saveBtn = new Fl_Button(x + 400, y, 150, 40, "Submit Replacement");
        Fl_Button* noneBtn = new Fl_Button(x + 500, y, 150, 40, "Don't Replace");
        Fl_Button* undoBtn = new Fl_Button(x + 600, y, 150, 40, "undo");
        replacementInputs.push_back(replacementInput);
        saveBtns.push_back(saveBtn);
        noneBtns.push_back(noneBtn);
        undoBtns.push_back(undoBtn);
        contextBoxes.push_back(contextDisplay);



        end();
    }

    submitBtn = new Fl_Button(x + 100, y + 75, 150, 40, "Submit All");

    drawThis();

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





void ContextPage::drawThis() {
    // MyPage::draw();
    for(auto box : contextBoxes) box->redraw();
    for(auto input : replacementInputs) input->redraw();
    for(auto btn : saveBtns) btn->redraw();
    for(auto btn : noneBtns) btn->redraw();
    for(auto btn : undoBtns) btn->redraw();
}

void ContextPage::initAttributes() {
    uSpaces = parent->getUSpaces();
    uPrintable = parent->getUPrintable();
    uNewLines = parent->getUNewLines();
    uPrintablePlus = parent->getUPrintablePlus();

    contextDictHere = parent->getContextDict();

    uPdfTextHere = parent->getUPdfText();
    uPdfListHere = parent->getUPdfList();
    newPdfTextHere = parent->getNewPdfText();
    newPdfListHere = parent->getNewPdfList();
    uBadChars = parent->getUBadChars();
    uCharOccurs = parent->getUCharOccurs();
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
                    // convert this UChar32 into a UnicodeString
                    (*contextDictHere)[thisChar][context] = thisChar;
                }
            }
        }
    }
}
// we could just call to choicePage but I've had enough issues with memory so this will do for now
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



