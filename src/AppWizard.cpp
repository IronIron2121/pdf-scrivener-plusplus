#include "AppWizard.h"
#include "FL/Fl_Wizard.H"
#include "OpenPDFPage.h"
#include "ChoicePage.h"
#include "ContextPage.h"

AppWizard::AppWizard(int w, int h, const char* title) : Fl_Window(w, h, title) {
    this->wizard = new Fl_Wizard(0, 0, w, h);
    this->openPage = new OpenPDFPage(0, 0, w, h, this, "PDF Scrivener - Open PDF");
    this->choicePage = new ChoicePage(0, 0, w, h, this, "PDF Scrivener - Choice Page");
    this->contextPage = new ContextPage(0, 0, w, h, this, "PDF Scrivener - Context Page");
    //uCharOccurs = new std::map<UChar32, int>;

    // string expressions of our lists of acceptable characters
    this->spaces = " ";
    this->printable = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|";
    this->newLines = "\n\f\t\v\r";
    this->printablePlus = printable + newLines + spaces;

    // functions to turn those strings into unordered sets of UChar32
    getSets(uSpaces, spaces);
    getSets(uPrintable, printable);
    getSets(uNewLines, newLines);
    getSets(uPrintablePlus, printablePlus);

    // initialise empty string to contain bad characters
    this->uBadChars = icu::UnicodeString::fromUTF8("");

    // initialise empty string to contain extracted text
    this->uPdfText = icu::UnicodeString::fromUTF8(""); // one big string, before cleaning
    this->newPdfText = icu::UnicodeString::fromUTF8(""); // one big string, after cleaning
    this->uPdfList = {}; // a list of string, before cleaning
    this->newPdfList = {}; // a list of string, after cleaning

    // initialise maps that will contain contexts in which bad characters occurs, and their respective replacements
    this->contextDict = {};
    this->replacementDict = {};
    this->contextList = {};

    // add the pages to the wizard
    this->wizard->add(openPage);
    this->wizard->add(choicePage);
    this->wizard->add(contextPage);

    // hide all pages except the first
    this->wizard->value(openPage);

    // integer to store which bad character we're on
    this->bIndex = 0;
    this->cIndex = 0;

    // end the wizard
    this->wizard->end();
}

AppWizard::~AppWizard(){
    std::cout << "AppWizard destructor called" << std::endl;
}

// returns uSPaces
std::unordered_set<UChar32>* AppWizard::getUSpaces() {
    return &(this->uSpaces);
}

std::unordered_set<UChar32>* AppWizard::getUNewLines() {
    return &(this->uNewLines);
}

std::unordered_set<UChar32>* AppWizard::getUPrintable() {
    return &(this->uPrintable);
}

std::unordered_set<UChar32>* AppWizard::getUPrintablePlus() {
    return &(this->uPrintablePlus);
}

void AppWizard::getSets(std::unordered_set<UChar32>& set, const std::string& stdStr) {
    // convert the string to a UnicodeString
    icu::UnicodeString uStr = icu::UnicodeString::fromUTF8(stdStr);
    for (int32_t i = 0; i < uStr.length();) {
        // get the character
        UChar thisChar = uStr.char32At(i);
        
        // push it into the set
        set.insert(uStr.char32At(i));

        // iterate by character length
        i += U16_LENGTH(thisChar);

    }
}

std::map<UChar32, int>* AppWizard::getUCharOccurs() {
    return &(this->uCharOccurs);
}

int AppWizard::getUCharOccur(UChar32 thisBadChar){
    return this->uCharOccurs[thisBadChar];
}

void AppWizard::upUCharOccur(UChar32 thisUChar) {
    this->uCharOccurs[thisUChar]++;
}

void AppWizard::upBIndex() {
    this->bIndex++;
}

UChar32 AppWizard::getCurrBadChar(){
    return this->uBadChars[this->bIndex];
}

icu::UnicodeString AppWizard::getGivenBadChar(int index) {
    return this->uBadChars[index];
}

bool AppWizard::endChecker(UChar32 thisChar, const icu::UnicodeString& enders) {
    // if this character is an ender, return true
    return enders.indexOf(thisChar) != -1;
}

std::pair<int32_t,int32_t> AppWizard::getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength){
    // list of characters that end a sentence
    icu::UnicodeString enders = u" .,\n";

    // initialise both pointers to the place where the bad character occurs
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

std::tuple<std::string, int, int, icu::UnicodeString> AppWizard::getConText(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    // error handling
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return std::make_tuple("", -1, -1, "");
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
    // get the size of the context
    int32_t contextSize = context.length();

    // combine positional information with context phrase, then convert to std::string
    icu::UnicodeString combined = posNotif + "\n" + context;
    std::string combinedStr;
    combined.toUTF8String(combinedStr);

    // pack together the context and the relative position of the bad character
    std::tuple<std::string, int, int, icu::UnicodeString> thisOut = std::make_tuple(combinedStr, leftDiff, contextSize, context);

    return thisOut;
}

// same principle as above, but returns only the unicode string
icu::UnicodeString AppWizard::getConTextC(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return icu::UnicodeString("N/A");
    }
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;
    int32_t leftDiff = indx - leftPointer;
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);
    return context;
}

void AppWizard::getContextsForRep(UChar32 thisChar) {
    std::cout << "GETTING CONTEXTS AT CONTEXT PAGE" << std::endl;
    // 1. for every page in the book
    int i = 0;
    for(const auto page : this->newPdfList){
        int32_t pageIt = 0;

        while(pageIt < page.length()){
            // i. if the current character is our current bad character
            UChar32 pageChar = page.charAt(pageIt);

            if (pageChar == thisChar) {
                // 1. get the context of the bad character here
                icu::UnicodeString context = this->getConTextC(pageIt, page);

                // 2. add this context to the list of contexts if it's not there already
                if (this->contextDict[thisChar].find(context) == this->contextDict[thisChar].end()) {
                    this->contextDict[thisChar][context] = thisChar;
                    // also add it to the vector of contexts
                    this->contextList[getCurrBadChar()].push_back(context);
                    // print from this index to test if it worked
                    icu::UnicodeString test = this->contextList[getCurrBadChar()][i];
                    std::string testStr;
                    test.toUTF8String(testStr);
                    std::cout << "TEST STRING: " << testStr << std::endl;
                    std::cout << "CONTEXT LIST UP TO SIZE: " << this->contextList[getCurrBadChar()].size() << std::endl;
                    i++;
                } else{
                    std::cout << "CONTEXT ALREADY IN DICT" << std::endl;
                }
            }
            pageIt += U16_LENGTH(pageChar);
        }
    }
}

// function to get a list of contexts in which a bad character occurs
std::vector<std::string> AppWizard::getListOfContexts() {
    // if bIndex is out of bounds, return an empty vector
    if (this->bIndex >= (this->uBadChars).length()) {
        std::cout << "bIndex out of bounds at getListOfContexts()" << std::endl;
        return {};
    } else{
        UChar32 currBadChar = getCurrBadChar();
        // get whichever is number is smaller - number of char occurences, or 3
        int numExamples = std::min((this->uCharOccurs)[currBadChar], 3);

        // var to store the indices of the bad characters
        std::vector<int32_t> indices;

        // try to .find() the bad character in the pdfText - starting from char 0
        int32_t minDex = 0;

        // for every example we wanna make
        for(int i = 0; i < numExamples; i++) {
            // try to find the next occurrence of this character
            int32_t thisDex = this->newPdfText.indexOf(currBadChar, minDex);

            // if we found it, add it to the list of indices
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::tuple<std::string, int, int, icu::UnicodeString> contOut= getConText(indices[i], this->newPdfText);
            std::string thisContext = std::get<0>(contOut);
            int thisPos = std::get<1>(contOut);
            int thisLength = std::get<2>(contOut);

            // create an underline the same length as the context
            std::string underLine(thisLength, '_');
            // turn position corresponding to bad character into an arrow
            underLine[thisPos] = '^';
            // add the underline to the context, then push context to list of contexts
            thisContext += "\n" + underLine;
            contextList.push_back(thisContext);
        }

        // return a vector containing the index of the bad character
        return contextList;
    }
}

std::string AppWizard::getDisplayChar() {
    // if there are no bad characters, return N/A
    if(!this->uBadChars.length()) {
        std::cout << "no bad characters" << std::endl;
        return "N/A";
    } else if (this->bIndex >= this->uBadChars.length()) {
        std::cout << "bIndex out of bounds at getDisplayChar()" << std::endl;
        return "N/A";
    }

    // convert current bad character to a unicode string
    icu::UnicodeString charToUString(getCurrBadChar());

    // convert that to a std string
    std::string uStringToStdString;
    charToUString.toUTF8String(uStringToStdString);
    
    // return the std::string
    return uStringToStdString;

}

icu::UnicodeString* AppWizard::getUPdfText(){
    return &(this->uPdfText);
}

void AppWizard::pushToUPdfText(icu::UnicodeString pageText){
    this->uPdfText.append(pageText);
}

icu::UnicodeString* AppWizard::getNewPdfText(){
    return &(this->newPdfText);
}

void AppWizard::pushToNewPdfText(icu::UnicodeString pageText){
    this->newPdfText += pageText;
}

std::vector<icu::UnicodeString>* AppWizard::getUPdfList(){
    return &(this->uPdfList);
}

void AppWizard::pushToUPdfList(icu::UnicodeString pageText){
    // get string version of text
    try{
        this->uPdfList.push_back(pageText);
    } catch (const std::exception& e) {
        std::cout << "exception at listPush: " << e.what() << std::endl;
    }
    std::cout << "success!" << std::endl;
}

std::vector<icu::UnicodeString>* AppWizard::getNewPdfList(){
    return &(this->newPdfList);
}

void AppWizard::pushToNewPdfList(icu::UnicodeString pageText){
    this->newPdfList.push_back(pageText);
}

std::map<UChar32, ReplacementInfo>* AppWizard::getReplacementDict(){
    return &(this->replacementDict);
}
std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* AppWizard::getContextDict(){
    return &(this->contextDict);
}

icu::UnicodeString* AppWizard::getUBadChars(){
    return &(this->uBadChars);
}

ChoicePage** AppWizard::getChoicePage(){
    return &(this->choicePage);
} 

ContextPage** AppWizard::getContextPage(){
    return &(this->contextPage);
} 

std::map<UChar32, std::vector<icu::UnicodeString>>* AppWizard::getContextList(){
    return &(this->contextList);
}

int* AppWizard::getBIndex() {
    return &(this->bIndex);
}

int* AppWizard::getCIndex(){
    return &(this->cIndex);
}

void AppWizard::upCIndex(){
    this->cIndex++;
}

void AppWizard::resetCIndex(){
    this->cIndex = 0;
}

icu::UnicodeString AppWizard::getCurrentContext(){
    return this->contextList[this->getCurrBadChar()][this->cIndex];
}

void AppWizard::doReplacements(){
    // Open a .txt file to write everything to
    // TODO - ADD PDF NAME HERE
    std::ofstream outFile("outputAll.txt");

    // catch opening errors
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output.txt for writing." << std::endl;
        return;
    }

    int i = 0;
    // Go through the book page by page and get replacements by searching in the map
    for (const auto& pageText : newPdfList) {
        i++;
        // a blank page to copy to
        icu::UnicodeString modText = icu::UnicodeString::fromUTF8("");
        
        // For every character in the page
        for (int32_t charIndex = 0; charIndex < pageText.length(); ) {
            // Grab the character
            UChar32 thisChar = pageText.char32At(charIndex);

            // if this char is printable
            if (uPrintable.find(thisChar) != uPrintable.end()) {
                modText += thisChar; // copy to output text
                charIndex += U16_LENGTH(thisChar);  // move over by the length of the character
            } 
            else if (uNewLines.find(thisChar) != uNewLines.end()) {
                // if it's a newline-like character, just add a newline (shouldn't be necessary, but good to have)
                UChar32 replacement = u'\n';
                modText += replacement;
                charIndex += U16_LENGTH(thisChar);  
                
            } 
            else {
                // otherwise it's a bad character
                // if it's non-contextual, replace it with its replacement
                if (!(replacementDict[thisChar].contextual)) {
                    // get the replacement and convert it to a UChar32
                    icu::UnicodeString replacement = (replacementDict)[thisChar].replacement;
                    modText += replacement;
                    charIndex += U16_LENGTH(thisChar);  
                } else{
                    // if it's contextual, find its context and provide the suitable replacement
                    // get conText tuple
                    std::tuple<std::string, int, int, icu::UnicodeString> conText = getConText(charIndex, pageText);
                    // just grab the context
                    icu::UnicodeString context = std::get<3>(conText);

                    // find the replacement associated with this context
                    icu::UnicodeString replacement = (contextDict)[thisChar][context];
                    modText += replacement; // add the replacement to output text
                    charIndex += U16_LENGTH(thisChar);  
                }
            }
        }
        
        // Convert the page to UTF8 and write to the file
        std::string utf8Page;
        modText.toUTF8String(utf8Page);
        outFile << utf8Page;
    }

    std::cout << "Done!" << std::endl;

    // Close the file
    outFile.close();

}