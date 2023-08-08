#include "AppWizard.h"

std::string AppWizard::accounted;
std::string AppWizard::printable;
std::string AppWizard::printablePlus;

AppWizard::AppWizard(int w, int h, const char* title) : Fl_Window(w, h, title) {
    wizard = new Fl_Wizard(0, 0, w, h);
    openPage = new OpenPDFPage(0, 0, w, h, this, "PDF Scrivener - Open PDF");
    choicePage = new ChoicePage(0, 0, w, h, this, "PDF Scrivener - Choice Page");
    charOccur = {};

    localPrintable = printablePlus;

    // initialise val for bad characters
    badChars = {};
    badCharsCh = {}; // list of bad characters

    pdfText = "";

    // add the pages to the wizard
    wizard->add(openPage);
    wizard->add(choicePage);

    // hide all pages except the first
    wizard->value(openPage);

    // integer to store which bad character we're on
    bindex = 0;
    
    accounted = "\f\t\v\r";
    printable = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n";
    printablePlus = printable + accounted;

    // end the wizard
    wizard->end();
}

int AppWizard::getBindex() {
    return bindex;
}

std::unordered_map<std::string, int>* AppWizard::getCharOccur() {
    return &charOccur;
}

void AppWizard::upBindex() {
    this->bindex++;
    std::cout << "bindex: " << bindex << std::endl;
}

std::string AppWizard::getBadChar() {
    return badChars[bindex];
}

std::string AppWizard::getGivenBadChar(int index) {
    return badChars[index];
}

bool AppWizard::endChecker(char thisChar, std::vector<char>& enders) {
    // if this character is an ender, return true
    return std::find(enders.begin(), enders.end(), thisChar) != enders.end();
}

std::string AppWizard::getConText(int indx, const std::string& pageText) {
    // if the index is out of bounds, run for the hills and cry because my code is perfect and it's your fault
    if (indx < 0 || indx >= pageText.size()) {
        std::cout << "bad index with: " << indx << std::endl;
        return "";
    }

    // characters that end a sentence
    std::vector<char> enders = {' ', '.', '\n', ','};

    // init context span indices
    int leftPointer = indx;
    int rightPointer = indx;

    // find the leftmost limit of the context
    while(!endChecker(pageText[leftPointer-1], enders) && leftPointer > 0) {
        leftPointer--;
    }

    // find the rightmost limit of the context
    while(!endChecker(pageText[rightPointer], enders) && rightPointer < pageText.size()-1) {
        rightPointer++;
    }
    
    // so we can find the position of the bad character in the context
    int leftDiff = indx - leftPointer;

    std::cout << "Just bad char:\n" << pageText.substr(indx,2) << std::endl;

    // bad character at position:
    std::string posNotif = "Pos: " + std::to_string(leftDiff+1) + " ";

    // add context to vector and return it
    std::string context = pageText.substr(leftPointer, rightPointer - leftPointer);

    // make a string of spaces the same length as context
    std::string invisiString(context.size(), '-');

    // add arrow pointing to the bad character
    if (leftDiff >= 0 && leftDiff < context.size()) {
        invisiString[leftDiff] = '^';
    } else {
        std::cerr << "bad char is out of bounds" << std::endl;
    }

    // combine and send off
    std::string combined = posNotif + "\n" + context + "\n" + invisiString;
    return combined;

}

// get character occurrences of a specific character
int AppWizard::getCharOccurs(std::string thisBadChar){
    return charOccur[thisBadChar];
}

std::string AppWizard::getDisplayChar() {
    // if there are no bad characters, return N/A
    if(!badChars.size()) {
        std::cout << "no bad characters" << std::endl;
        return "N/A";
    } else if (bindex >= badChars.size()) {
        std::cout << "bindex out of bounds at getDisplayChar()" << std::endl;
        return "N/A";
    }
    std::string realBadChar = badChars[bindex];
    return realBadChar;

}


/*
// using ICU, find a 

// extract a slice around the bad character
std::string slice = pageList[pageInt].substr(charIt, 4);
// convert the slice to a unicode string
icu::UnicodeString uStr(slice.c_str(), slice.size(), "UTF-8"); 
// get the first (i.e. bad) character in the unicode string
UChar32 uChar = uStr.char32At(0);

// convert the unicode char back to a string...
// extract a UTF8 substring of length (this bad character) from the unicode string
// ...I know, I'm exhausted too
std::string backToStr;
uStr.tempSubString(0, U16_LENGTH(uChar)).toUTF8String(backToStr);

// if it's not already been added to the bad character list
if(std::find(badChars->begin(), badChars->end(), backToStr) == badChars->end()) {
badChars->push_back(backToStr); // add to bad character list
}
// iterate forwards, past the unicode character
charIt += U16_LENGTH(uChar) - 1;
parentHere->upCharOccur(backToStr);// add to number of this character's occurrences
*/

// check again printable to find non-good characters, then use ICU to check which bad character it is
// if it's the one that we're looking for, then get the context around it


std::vector<std::string> AppWizard::getBintexts() {
    // if bindex is out of bounds, return an empty vector
    if (bindex >= badChars.size()) {
        std::cout << "bindex out of bounds at getBintexts()" << std::endl;
        return {};
    } else{
        // otherwise, get the current bad character
        std::string realBadChar = badChars[bindex];
        // get whichever is smaller - number of char occurences, or 3
        int numExamples = std::min((charOccur[realBadChar]), 3);

        std::cout << "numExamples: " << numExamples << std::endl;
        std::cout << "realBadChar: " << realBadChar << std::endl;
        std::cout << "pdfText size: " << pdfText.size() << std::endl;

        // vars to store the indices of the bad characters
        std::vector<int> indices; // where we'll store character indices
        std::string outString; // where we'll store context

        int i = 0; // start from 0

        // try to .find() the bad character in the pdfText
        int minDex = 0;

        for(int i = 0; i < numExamples; i++) {
            int thisDex = pdfText.find(realBadChar, minDex);
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }


        /*

        std::cout << (i < numExamples && i < pdfText.size()) << std::endl;
        // check pdfText for non-printable characters, use ICU to check which bad character it is if it's the one we're looking for, then save its index
        while(indices.size() < numExamples && i < pdfText.size()){
            std::cout << pdfText[i] << std::endl;
            if (printablePlus.find(pdfText[i]) == std::string::npos){
                std::cout << "found non-printable character at index: " << i << std::endl;
                // find rightmost limit of this slice
                int sliceIt = i; 
                while (sliceIt < pdfText.size() && printablePlus.find(pdfText[sliceIt]) == std::string::npos ) {
                    sliceIt++;
                }
                // extract a slice around the bad character
                std::string slice = pdfText.substr(i, sliceIt - i);
                // convert the slice to a unicode string
                icu::UnicodeString uStr(slice.c_str(), slice.size(), "UTF-8"); 
                // get the first (i.e. bad) character in the unicode string
                UChar32 uChar = uStr.char32At(0);

                // convert the unicode char back to a string...
                // extract a UTF8 substring of length (this bad character) from the unicode string
                std::string backToStr;
                uStr.tempSubString(0, U16_LENGTH(uChar)).toUTF8String(backToStr);

                // if it's the one we're looking for, add its index to our index vector and 
                if(backToStr == realBadChar) {
                    std::cout << "found bad char at index: " << i << std::endl;
                    indices.push_back(i);
                }
                // iterate forwards, past the unicode character
                i += U16_LENGTH(uChar) - 1;
            } else{
                // if it's a printable character, just iterate forwards
                i++;
            }
        }
        */

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::string thisContext = getConText(indices[i], pdfText);
            contextList.push_back(thisContext);
        }

        // return a vector containing the index of the bad character
        return contextList;
    }
}

// function to update char occurrences given a sent char
void AppWizard::upCharOccur(std::string thisChar) {
    charOccur[thisChar]++;
}

std::string* AppWizard::getPdfText(){
    return &pdfText;
}



// refresh the values of the choice page when next is pressed
void AppWizard::refreshVals(void* data) {
    // update bad char display
    std::string newText = "Current Character: " + std::string(this->getDisplayChar());
    std::cout << "new text: " << newText << std::endl;

    Fl_Box *thisCharLabel = this->choicePage->getCharLabel();
    thisCharLabel->copy_label(newText.c_str());


    // grab the character context boxes
    std::vector<Fl_Box*> chartextBoxes = this->choicePage->getChartextBoxes();

    // get the contexts for the current bad character
    std::vector<std::string> listOfContexts = this->getBintexts();


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

std::vector<std::string>* AppWizard::getPdfPages(){
    return &pdfPages;
}

void AppWizard::pushToPdfPages(std::string pageText){
    pdfPages.push_back(pageText);
}


std::vector<std::string>* AppWizard::getBadChars(){
    return &(badChars);
}

std::string AppWizard::getLocalPrintable() {
    return localPrintable;
}

