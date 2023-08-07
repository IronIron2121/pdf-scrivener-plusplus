#include "AppWizard.h"

AppWizard::AppWizard(int w, int h, const char* title) : Fl_Window(w, h, title) {
    wizard = new Fl_Wizard(0, 0, w, h);
    openPage = new OpenPDFPage(0, 0, w, h, this, "PDF Scrivener - Open PDF");
    choicePage = new ChoicePage(0, 0, w, h, this, "PDF Scrivener - Choice Page");

    pdfText = "";

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

char AppWizard::getGivenBadChar(int index) {
    return badChars[index];
}

std::string AppWizard::getConText(int indx, const std::string& pageText) {
    if (indx < 0 || indx >= pageText.size()) {
        std::cout << "bad index. how did this even happen?" << std::endl;
        return "";
    } else{
        std::cout << "good index" << std::endl;
    }

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
    while(!endChecker(pageText[rightPointer], enders) && rightPointer < pageText.size()-1) {
        rightPointer++;
    }
    
    // show the left and right pointers
    std::cout << "leftPointer: " << leftPointer << std::endl;
    std::cout << "rightPointer: " << rightPointer << std::endl;
    // show pagetext length
    std::cout << "pagetext length: " << pageText.size() << std::endl;



    // add the context to the vector and return it
    std::string context = pageText.substr(leftPointer, rightPointer - leftPointer);
    return context;
}

int AppWizard::getCharOccurs(char thisBadChar){
    return charOccur[thisBadChar];
}

std::vector<std::string> AppWizard::getBintexts(char thisBadChar) {
    // get whichever is smaller - number of char occurences, or 3
    int numExamples = std::min(this->getCharOccurs(thisBadChar), 3);
    std::cout << "numExamples: " << numExamples << std::endl;
    std::cout << "thisBadChar: " << thisBadChar << std::endl;

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
        std::cout << "adding contexts at index " << i << std::endl;
        contextList.push_back(getConText(indices[i], pdfText));
    }

    // return a vector containing the index of the bad character
    return contextList;
}

// function to update char occurrences given a sent char
void AppWizard::upCharOccur(char thisChar) {
    charOccur[thisChar]++;
}



bool AppWizard::endChecker(char thisChar, std::vector<char>& enders) {
    // if this character is an ender, return true
    return (std::find(enders.begin(), enders.end(), thisChar) != enders.end());}

void AppWizard::refreshVals(void* data) {
    std::cout << "refreshing values of display\n" << std::endl;
    // update bad char display
    std::string newText = "Current Character: " + std::string(1, this->getBadChar());
    this->choicePage->getCharLabel()->copy_label(newText.c_str());

    std::cout << "refreshing values of boxes\n" << std::endl;
    // grab the character context boxes
    std::vector<Fl_Box*> chartextBoxes = this->choicePage->getChartextBoxes();

    std::cout << "getting list of contexts" << std::endl;
    std::vector<std::string> listOfContexts = this->getBintexts(this->bindex);
    std::cout << "just got the contexts, with size " << listOfContexts.size() << std::endl;

    // update contexts display
    for (int i = 0; i < listOfContexts.size() && i < chartextBoxes.size(); i++) {
        std::cout << "copying label at index " << i << std::endl;
        std::cout << "label: " << listOfContexts[i].c_str() << std::endl;
        chartextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
}

std::string* AppWizard::getBadChars(){
    return &(badChars);
}

