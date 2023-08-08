#include "AppWizard.h"


AppWizard::AppWizard(int w, int h, const char* title) : Fl_Window(w, h, title) {
    wizard = new Fl_Wizard(0, 0, w, h);
    openPage = new OpenPDFPage(0, 0, w, h, this, "PDF Scrivener - Open PDF");
    choicePage = new ChoicePage(0, 0, w, h, this, "PDF Scrivener - Choice Page");
    //uCharOccurs = new std::unordered_map<UChar32, int>;

    uSpaces = icu::UnicodeString::fromUTF8(" ");
    uPrintable = icu::UnicodeString::fromUTF8(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n");
    uNewLines = icu::UnicodeString::fromUTF8("\n\f\t\v\r");
    uPrintablePlus = uPrintable + uNewLines + uSpaces;

    // initialise val for bad characters
    uBadChars = icu::UnicodeString::fromUTF8("");
    badCharsCh = {}; // list of bad characters

    // initialise our containers and whatnot
    uPdfText = icu::UnicodeString::fromUTF8("");
    newPdfText = icu::UnicodeString::fromUTF8("");
    uPdfList = {};
    newPdfList = {};

    replacementDict = {};

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

// returns uSPaces
icu::UnicodeString AppWizard::getUSpaces() {
    return uSpaces;
}
// returns uNewLines characters
icu::UnicodeString AppWizard::getUNewLines() {
    return uNewLines;
}
// returns uPrintable characters
icu::UnicodeString AppWizard::getUPrintable() {
    return uPrintable;
}

icu::UnicodeString AppWizard::getUPrintablePlus() {
    return uPrintablePlus;
}

std::unordered_map<UChar32, int>* AppWizard::getUCharOccurs() {
    return &(uCharOccurs);
}
// get character occurrences of a specific character
int AppWizard::getUCharOccur(UChar32 thisBadChar){
    return uCharOccurs[thisBadChar];
}


void AppWizard::upBindex() {
    this->bindex++;
    std::cout << "bindex: " << bindex << std::endl;
}

UChar32 AppWizard::getBadChar() {
    return uBadChars[bindex];
}

icu::UnicodeString AppWizard::getGivenBadChar(int index) {
    return uBadChars[index];
}

bool AppWizard::endChecker(UChar32 thisChar, const icu::UnicodeString& enders) {
    // if this character is an ender, return true
    return enders.indexOf(thisChar) != -1;
}
std::pair<int32_t,int32_t> AppWizard::getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength){
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
std::string AppWizard::getConText(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    if (indx < 0 || indx >= thisPageLength) {
        std::cout << "bad index with: " << indx << std::endl;
        return "N/A";
    }

    // get the pointers for this run and assign them
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;

    // get the relative position of the bad character
    int32_t leftDiff = indx - leftPointer;
    // bad character at position:
    icu::UnicodeString posNotif = "Pos: ";
    icu::UnicodeString strNumber = icu::UnicodeString::fromUTF8(std::to_string(leftDiff + 1));
    posNotif += strNumber;
    posNotif += " ";

    // get the context and make a string to underline it
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);
    icu::UnicodeString invisiString(context.length(), '-');

    // add arrow pointing to the bad character
    if (leftDiff >= 0 && leftDiff < context.length()) {
        invisiString.setCharAt(leftDiff, '^');
    } else {
        std::cerr << "bad char is out of bounds" << std::endl;
    }

    // combine, convert to std::string, and return
    icu::UnicodeString combined = posNotif + "\n" + context + "\n" + invisiString;
    std::string combinedStr;
    combined.toUTF8String(combinedStr);

    return combinedStr;
}
std::vector<std::string> AppWizard::getBintexts() {
    // if bindex is out of bounds, return an empty vector
    if (bindex >= uBadChars.length()) {
        std::cout << "bindex out of bounds at getBintexts()" << std::endl;
        return {};
    } else{
        // otherwise, get the current bad character
        UChar32 realBadChar = uBadChars[bindex];
        // get whichever is number is smaller - number of char occurences, or 3
        int numExamples = std::min(uCharOccurs[realBadChar], 3);

        // debugging prints
        std::cout << "numExamples: " << numExamples << std::endl;
        std::cout << "realBadChar: " << realBadChar << std::endl;
        std::cout << "pdfText size: " << uPdfText.length() << std::endl;

        // vars to store the indices of the bad characters
        std::vector<int32_t> indices; // where we'll store character indices

        // try to .find() the bad character in the pdfText - starting from char 0
        int32_t minDex = 0;
        for(int i = 0; i < numExamples; i++) {
            // try to find the next occurrence of this character
            int32_t thisDex = uPdfText.indexOf(realBadChar, minDex);

            // if we found it, add it to the list of indices
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::string thisContext = getConText(indices[i], uPdfText);
            contextList.push_back(thisContext);
        }

        // return a vector containing the index of the bad character
        return contextList;
    }
}


std::string AppWizard::getDisplayChar() {
    // if there are no bad characters, return N/A
    if(!uBadChars.length()) {
        std::cout << "no bad characters" << std::endl;
        return "N/A";
    } else if (bindex >= uBadChars.length()) {
        std::cout << "bindex out of bounds at getDisplayChar()" << std::endl;
        return "N/A";
    }
    // grab the character
    UChar32 realBadChar = uBadChars[bindex];
    // convert it to a unicode string
    icu::UnicodeString charToUString(realBadChar);
    // convert that to a std string, then return
    std::string uStringToStdString;
    charToUString.toUTF8String(uStringToStdString);
    return uStringToStdString;

}



// function to update char occurrence given a sent character
void AppWizard::upUCharOccur(UChar32 thisUChar) {
    uCharOccurs[thisUChar]++;
}





// refresh the values of the choice page when next is pressed
void AppWizard::refreshVals() {
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

// -- GET AND MODIFY FUNCTIONS FOR INITIAL AND PROCESSED PDF -- //
icu::UnicodeString* AppWizard::getUPdfText(){
    return &uPdfText;
}
void AppWizard::pushToUPdfText(icu::UnicodeString pageText){
    uPdfText += pageText;
}
icu::UnicodeString* AppWizard::getNewPdfText(){
    return &newPdfText;
}
void AppWizard::pushToNewPdfText(icu::UnicodeString pageText){
    newPdfText += pageText;
}
// ------------------------------------------------------------- //

// -- GET AND MODIFY FUNCTIONS FOR INITIAL AND PROCESSED PDF, PAGE BY PAGE -- //
std::vector<icu::UnicodeString>* AppWizard::getUPdfList(){
    return &uPdfList;
}
void AppWizard::pushToUPdfList(icu::UnicodeString pageText){
    uPdfList.push_back(pageText);
}
std::vector<icu::UnicodeString>* AppWizard::getNewPdfList(){
    return &newPdfList;
}
void AppWizard::pushToNewPdfList(icu::UnicodeString pageText){
    newPdfList.push_back(pageText);
}
// ------------------------------------------------------------------------- //

// functions for replacement choices
void AppWizard::goodifyRep(){
    // non-contextual, and its replacement is itself
    replacementDict[getBadChar()].contextual = false;
    replacementDict[getBadChar()].replacement = getBadChar();   
}
void AppWizard::replaceAllRep(UChar32 replacementChar){
    // non-contextual, and its replacement is given value
    replacementDict[getBadChar()].contextual = false;
    replacementDict[getBadChar()].replacement = replacementChar; 
    
}
void AppWizard::contextualRep(){
    replacementDict[getBadChar()].contextual = false;
// TODO: CONTEXTUAL REPLACEMENT
//    replacementDict[getBadChar()].replacement = ();    
}
void AppWizard::echoReplacement(){
    std::cout << "replacement: " << replacementDict[getBadChar()].replacement << std::endl;
}

icu::UnicodeString* AppWizard::getUBadChars(){
    return &(uBadChars);
}

void AppWizard::doReplacements() {
    // Open a .txt file to write everything to
    // TODO - ADD PDF NAME
    std::ofstream outFile("outputAll.txt");

    // catch opening errors
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output.txt for writing." << std::endl;
        return;
    }

    // Go through the book page by page and get replacements by searching in the map
    for (const auto pageText : uPdfList) {
        // a blank page to copy to
        icu::UnicodeString modText = icu::UnicodeString::fromUTF8("");
        
        // For every character in the page
        for (int32_t charIndex = 0; charIndex < pageText.length(); ) {
            // Grab the character
            UChar32 thisChar = pageText.char32At(charIndex);

            // if this char is printable
            if (uPrintable.indexOf(thisChar) != -1) {
                // simply copy over and move over by its size
                modText += thisChar;
                charIndex += U16_LENGTH(thisChar);  // Move by the length of the character
            } else if (uNewLines.indexOf(thisChar) != -1) {
                // if it's a newline-like character, just add a newline
                UChar32 replacement = u'\n';
                modText += replacement;
                charIndex += U16_LENGTH(replacement);  // move over by newline length
                
                }else {
                // otherwise, replace it with its replacement
                icu::UnicodeString replacement = icu::UnicodeString::fromUTF8(replacementDict[thisChar].replacement);
                modText += replacement;
                charIndex += replacement.length();  // you know the drill
            }
        }
        
        // Convert the page to UTF8 and write to the file
        std::string utf8Page;
        modText.toUTF8String(utf8Page);
        outFile << utf8Page;
    }

    // Close the file
    outFile.close();
}

