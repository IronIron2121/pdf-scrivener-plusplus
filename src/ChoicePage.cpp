#include "ChoicePage.h"
#include "AppWizard.h"

struct ReplacementInfo {
    bool contextual; // is this replacement context sensitive?
    std::string replacement; // what is the replacement?
};
std::unordered_map<char, ChoicePage::ReplacementInfo> ChoicePage::replacementDict;

ChoicePage::ChoicePage(int x, int y, int w, int h, const char* title, AppWizard* parent) 
    : MyPage(x, y, w, h, title) {
        // init replacement info and dictionary
        

        // Display current bad character
        std::string charText = "Current Character: " + std::string(1, parent->getBadChar());
        thisCharLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());

        // Display context for the bad character
        std::vector<std::string> listOfContexts = parent->getBintexts(parent->getBindex());
        // gap between each context
        int yGap = 50;
        // create a box for each context
        for (int context = 0; context < listOfContexts.size(); context++) {
            Fl_Box* box = new Fl_Box(x+10, y+yGap, w-20, 30, listOfContexts[context].c_str());
            chartextBoxes.push_back(box);
            yGap += 40;
        }

        // Buttons for actions
        goodifyButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Do not replace this character");
        goodifyButton->callback(goodifyCb, parent);

        yGap += 50;
        replaceAllButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Replace all instances of this character with input below");
        replaceAllInput = new Fl_Input(x+10, y+yGap+25, w-20, 40, "Replacement: ");
        replaceAllButton->callback(replaceAllCb, parent);

        yGap += 50;
        contextButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Choose a different replacement dependent on character context");
        contextButton->callback(contextCb, parent);

        end();
}


Fl_Box* ChoicePage::getCharLabel() {
    return thisCharLabel;
}

std::vector<Fl_Box*> ChoicePage::getChartextBoxes() {
    return chartextBoxes;
}

void ChoicePage::goodifyCb(Fl_Widget* w, void* data) { 
    // show em who's daddy
    AppWizard* parent = (AppWizard*)data;
    // make this character's replacement itself, declare it as non-contextual
    replacementDict[parent->getBadChar()].replacement = parent->getBadChar();
    replacementDict[parent->getBadChar()].contextual = false;
    nextChar(w, data);}

void ChoicePage::replaceAllCb(Fl_Widget* w, void* data) {
    // show em who's daddy
    AppWizard* parent = (AppWizard*)data;
    ChoicePage* thisPage = (ChoicePage*)w->parent();
    // replace every instance of this character with the user's input
    replacementDict[parent->getBadChar()].replacement = thisPage->replaceAllInput->value();
    replacementDict[parent->getBadChar()].contextual = false;
}

void ChoicePage::contextCb(Fl_Widget* w, void* data) {
    AppWizard* parent = (AppWizard*)data;
}

void ChoicePage::nextChar(Fl_Widget* w, void* data) {
    // show em who's daddy (yes, it's that funny)
    AppWizard* parent = (AppWizard*)data;
    // increment the bad character index
    parent->upBindex();
    // if the bad character index is greater than the number of bad characters
    if(parent->getBindex() >= (*parent->getBadChars()).size()) {
        // start replacing the bad characters
        doReplacements();
    } else {
        // otherwise refresh the page values
        parent->refreshVals(data); 
    }
}

void ChoicePage::doReplacements() {
}