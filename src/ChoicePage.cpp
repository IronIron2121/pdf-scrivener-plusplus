#include "ChoicePage.h"
#include "AppWizard.h"
#include "ContextPage.h"
#include <iostream>


ChoicePage::ChoicePage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // init parent and parameter attributes
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;

    // init other attributes and pointers
    this->initPointers();
    this->initAttributes();
    this->initDisplays();

    this->end();
}

void ChoicePage::initAttributes(){
    // packs of attributes for button callbacks
    this->pack0 = new choicePack;
    this->pack0->choice = 0;
    this->pack0->instance = this;

    this->pack1 = new choicePack;
    this->pack1->choice = 1;
    this->pack1->instance = this;

    this->pack2 = new choicePack;
    this->pack2->choice = 2;
    this->pack2->instance = this;

    // y-increment gap between each button
    this->yGap = 100;
}

void ChoicePage::initPointers(){
    this->bIndexHere = this->parent->getBIndex();
    this->uBadCharsHere = this->parent->getUBadChars();
    this->replacementDictHere = this->parent->getReplacementDict();
    this->contextPageHere = this->parent->getContextPage();
}

void ChoicePage::initDisplays(){
    // Display current bad character
    this->displayChar = "N/A";
    this->charText = "Character: ";
    this->charLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());

    // create a box for each context
    for (int contextIt = 0; contextIt < 3; contextIt++) {
        this->y = y+yGap;
        Fl_Box* box = new Fl_Box(this->x+10, this->y, this->w, 70);
        this->charTextBoxes.push_back(box);
    }

    this->y = this->y + this->yGap; // increment y

    // Buttons for actions
    // button to replace all instances of this character with itself
    this->goodifyButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Do not replace this character");
    this->goodifyButton->callback(this->activateChoiceClick, this->pack0);

    this->y = this->y + this->yGap; 

    // button to replace all instances of this character with given input
    this->replaceAllButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Replace all instances of this character with input below");
    this->replaceAllInput = new Fl_Input(this->x+10, this->y+25, this->w-20, 40, "Replacement: ");
    this->replaceAllButton->callback(this->activateChoiceClick, this->pack1);

    this->y = this->y + this->yGap;

    // button to replace this character with a different character depending on context it appears in
    this->contextButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Choose a different replacement dependent on character context");
    this->contextButton->callback(this->activateChoiceClick, this->pack2);
}

void ChoicePage::activateChoiceClick(Fl_Widget* widget, void* data){
    // get the passed pack, and unpack it
    choicePack* pack = static_cast<choicePack*>(data);
    int choice = pack->choice;
    ChoicePage* thisPage = pack->instance;

    // get the current bad character
    thisPage->currBadChar = thisPage->parent->getCurrBadChar();

    // trigger event based on choices
    if(choice == 0){
        thisPage->goodifyCb();
    } else if(choice == 1){
        thisPage->replaceAllCb();
    } else if(choice == 2){
        thisPage->contextCb();
    }
}

void ChoicePage::goodifyCb() { 
    // non-contextual replacement 
    (*(this->replacementDictHere))[this->currBadChar].contextual = false;

    // replacement is the character itself, converted from uChar to uString
    icu::UnicodeString thisRep = icu::UnicodeString(this->currBadChar);
    (*(this->replacementDictHere))[this->currBadChar].replacement = thisRep; 

    nextChar(); // go to next character
}


void ChoicePage::replaceAllCb() {
    // non-contextual replacement
    (*(this->replacementDictHere))[this->currBadChar].contextual = false;

    // replace every instance of this character with the user's input
    std::string inputStr = this->replaceAllInput->value();
    icu::UnicodeString inputU(inputStr.c_str()); // convert input to unicode string

    // put it in the replacement dictionary
    (*(this->replacementDictHere))[this->currBadChar].replacement = inputU;
    
    nextChar(); // go to next character

}

void ChoicePage::contextCb() {
    // contextual replacement
    (*(this->replacementDictHere))[this->currBadChar].contextual = true;

    // hide this page, initialise the context page, and show it
    this->hide();
    this->parent->contextPage->newInit();
    this->parent->contextPage->show();
}

void ChoicePage::nextChar() {
    // increment the bad character index
    this->parent->upBIndex();

    // if the bad character index is greater than the number of bad characters
    if(*(this->bIndexHere) >= this->uBadCharsHere->length()) {
        // start replacing the bad characters
        this->parent->doReplacements();
    } else {
        // otherwise refresh the page values
        this->refreshVals(); 
    }
}

void ChoicePage::refreshVals() {
    // update bad char display
    std::string newText = "Character " + std::to_string((this->parent->bIndex) + 1) + "/" + std::to_string(this->uBadCharsHere->length()) + ": " + std::string(this->parent->getDisplayChar() + ", with " + std::to_string(this->parent->getUCharOccur(this->parent->getCurrBadChar())) + " occurrences");
    this->charLabel->copy_label(newText.c_str());

    // get a list of contexts for the current bad character
    std::vector<std::string> listOfContexts = this->parent->getListOfContexts();

    // get the size of the list and the boxes    
    int listSize = listOfContexts.size();
    int boxSize = charTextBoxes.size();

    // update contexts display
    for (int i = 0; i < listOfContexts.size(); i++) {
        this->charTextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
    // if there's an empty box, fill it with N/A
    if (listSize < boxSize) {
        for (int i = listSize; i < boxSize; i++) {
            this->charTextBoxes[i]->copy_label("N/A");
        }
    }
}

