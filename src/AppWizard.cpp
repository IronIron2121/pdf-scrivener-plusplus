#include "AppWizard.h"

AppWizard::AppWizard(int w, int h, const char* title) : Fl_Window(w, h, title) {
    wizard = new Fl_Wizard(0, 0, w, h);
    openPage = new OpenPDFPage(0, 0, w, h, "PDF Scrivener - Open PDF");
    choicePage = new ChoicePage(0, 0, w, h, "PDF Scrivener - Choice Page", this);

    std::unordered_map<char, int> charOccur; // every char and its occurences
    charOccur['A']++;


    // add the pages to the wizard
    wizard->add(openPage);
    wizard->add(choicePage);

    // hide all pages except the first
    wizard->value(openPage);

    // integer to store which bad character we're on
    bindex = 0;
    
    // end the wizard
    wizard->end();
}

int AppWizard::getBindex() {
    return bindex;
}

std::unordered_map<char, int>* AppWizard::getCharOccur() {
    return &charOccur;
}

void AppWizard::upBindex() {
    this->bindex++;
}

char AppWizard::getBadChar() {
    return badChars[bindex];
}

std::vector<std::string> AppWizard::getConText(int indx, std::string& pageText) {
    // initialise vector for context of bad characters
    std::vector<std::string> conText;

    // characters that end a sentence
    std::vector<char> enders = {' ', '.', '\n'};

    // init context span indices
    int leftPointer = indx;
    int rightPointer = indx;

    // find the leftmost limit of the context
    while(!endChecker(pageText[leftPointer], enders) && leftPointer > 0) {
        leftPointer--;
    }

    // find the rightmost limit of the context
    while(!endChecker(pageText[rightPointer], enders) && rightPointer < pageText.size()) {
        rightPointer++;
    }

    // add the context to the vector and return it
    std::string context = pageText.substr(leftPointer, rightPointer - leftPointer);
    return conText;
}

std::vector<std::string> AppWizard::getBintexts(char thisBadChar) {
    // get whichever is smaller - number of char occurences, or 3
    int numExamples = std::min(this->charOccur[thisBadChar], 3);

    // vars to store the indices of the bad characters
    std::vector<int> indices; // where we'll store character indices
    std::string outString; // where we'll store context
    int minDex = 0; // start from 0

    for(int i = 0; i < numExamples; i++) {
        // get the index of the next occurrence of the bad character
        int thisDex = pdfText.find(thisBadChar, minDex);
        // push it to the vector
        indices.push_back(thisDex);
        // set the minimum index to the next index 
        minDex = thisDex + 1;
    }

    // get the context of each bad character
    std::vector<std::string>contextList;
    for(int i = 0; i < indices.size(); i++) {
        contextList.push_back(getConText(indices[i], pdfText)[0]);
    }

    // return a vector containing the index of the bad character
    return contextList;
}

bool AppWizard::endChecker(char thisChar, std::vector<char>& enders) {
    // if this character is an ender, return true
    return (std::find(enders.begin(), enders.end(), thisChar) != enders.end());}

void AppWizard::refreshVals(void* data) {
    // update bad char display
    std::string newText = "Current Character: " + std::string(1, this->getBadChar());
    this->choicePage->getCharLabel()->copy_label(newText.c_str());

    std::vector<Fl_Box*> chartextBoxes = this->choicePage->getChartextBoxes();

    // update contexts display
    std::vector<std::string> listOfContexts = this->getBintexts(this->bindex);
    for (int i = 0; i < listOfContexts.size() && i < chartextBoxes.size(); i++) {
        chartextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
}

std::string* AppWizard::getBadChars(){
    return &(badChars);
}

