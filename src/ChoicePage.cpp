#include "ChoicePage.h"
#include "AppWizard.h"

ChoicePage::ChoicePage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // Display current bad character
    std::string charText = "Current Character: " + (parent->getDisplayChar());
    thisCharLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());

    // Display context for the bad character
    std::vector<std::string> listOfContexts = parent->getBintexts();

    finalString = "";
    
    // gap between each context
    int yGap = 100;
    
    // create a box for each context
    for (int context = 0; context < 3; context++) {
        y = y+yGap;
        Fl_Box* box = new Fl_Box(x+10, y, w, 70);
        chartextBoxes.push_back(box);
    }

    y = y+yGap;

    choicePack* pack0 = new choicePack;
    pack0->choice = 0;
    pack0->instance = this;
    choicePack* pack1 = new choicePack;
    pack1->choice = 1;
    pack1->instance = this;
    choicePack* pack2 = new choicePack;
    pack2->choice = 2;
    pack2->instance = this;

    // Buttons for actions
    goodifyButton = new Fl_Button(x+10, y, w-20, 40, "Do not replace this character");
    goodifyButton->callback(activateChoiceClick, pack0);

    y = y+yGap;
    replaceAllButton = new Fl_Button(x+10, y, w-20, 40, "Replace all instances of this character with input below");
    replaceAllInput = new Fl_Input(x+10, y+25, w-20, 40, "Replacement: ");
    replaceAllButton->callback(activateChoiceClick, pack1);\

    y = y+yGap;
    contextButton = new Fl_Button(x+10, y, w-20, 40, "Choose a different replacement dependent on character context");
    contextButton->callback(activateChoiceClick, pack2);

    end();
}


Fl_Box* ChoicePage::getCharLabel() {
    return thisCharLabel;
}

std::vector<Fl_Box*> ChoicePage::getChartextBoxes() {
    return chartextBoxes;
}

void ChoicePage::activateChoiceClick(Fl_Widget* w, void* data){
    // get the passed pack, and unpack it
    choicePack* pack = static_cast<choicePack*>(data);
    int choice = pack->choice;
    ChoicePage* thisPage = pack->instance;

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
    // make this character's replacement itself, declare it as non-contextual
    parent->goodifyRep();
    parent->echoReplacement();
    nextChar();
}

void ChoicePage::replaceAllCb() {
    // replace every instance of this character with the user's input
    this->parent->replaceAllRep(this->replaceAllInput->value()[0]);
    this->parent->echoReplacement();
}

void ChoicePage::contextCb() {
    // TODO
}

void ChoicePage::nextChar() {
    // increment the bad character index
    parent->upBindex();
    // if the bad character index is greater than the number of bad characters
    if(parent->getBindex() >= parent->getUBadChars()->length()) {
        std::cout << "Going for replacements!" << std::endl;
        // start replacing the bad characters
        this->parent->doReplacements();
    } else {
        // otherwise refresh the page values
        this->parent->refreshVals(); 
    }
}
