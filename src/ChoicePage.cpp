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
    // Buttons for actions
    goodifyButton = new Fl_Button(x+10, y, w-20, 40, "Do not replace this character");
    goodifyButton->callback(goodifyCb, parent);

    y = y+yGap;
    replaceAllButton = new Fl_Button(x+10, y, w-20, 40, "Replace all instances of this character with input below");
    replaceAllInput = new Fl_Input(x+10, y+25, w-20, 40, "Replacement: ");
    replaceAllButton->callback(replaceAllCb, parent);\

    y = y+yGap;
    contextButton = new Fl_Button(x+10, y, w-20, 40, "Choose a different replacement dependent on character context");
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
    parent->goodifyRep();
    std::cout << "replacement: " << parent->replacementDict[parent->getBadChar()].replacement << std::endl;
    nextChar(w, data);}

void ChoicePage::replaceAllCb(Fl_Widget* w, void* data) {
    // show em who's daddy
    AppWizard* parent = (AppWizard*)data;
    ChoicePage* thisPage = (ChoicePage*)w->parent();
    // replace every instance of this character with the user's input
    replacementDict[parent->getBadChar()].replacement = thisPage->replaceAllInput->value();
    replacementDict[parent->getBadChar()].contextual = false;
    std::cout << "replacement: " << replacementDict[parent->getBadChar()].replacement << std::endl;
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
        std::cout << "Going for replacements!" << std::endl;
        // start replacing the bad characters
        doReplacements(parent);
    } else {
        // otherwise refresh the page values
        parent->refreshVals(data); 
    }
}

void ChoicePage::doReplacements(void* data) {
    // open a .txt file to write to
    std::ofstream outFile;
    outFile.open("output.txt");
    // go through the book page by page and get replacements by searching in map
    AppWizard* parent = (AppWizard*)data;
    std::string uPrintable = parent->getLocalPrintable();
    std::vector<std::string>* pdfPages = parent->getPdfPages();
    // for every page in the book
    for(int page = 0; page < pdfPages->size(); page++) {
        // grab the page text
        std::string pageText = (*pdfPages)[page];

        // for every character in the page
        for(int charIndex = 0; charIndex < pageText.size(); charIndex++) {
            // grab the character
            std::string thisChar = std::string(1, pageText[charIndex]);
            // if this character is uPrintable, skip it
            if(uPrintable.find(thisChar) != std::string::npos) {
                continue;
            } else{
                // otherwise, replace it with its replacement
                std::string replacement = replacementDict[thisChar].replacement;
                pageText.replace(charIndex, 1, replacement);
            
        }
    }
    // write the page to the file
    outFile << pageText;
    // end your long-suffering nightmare
    outFile.close();
    }
}


