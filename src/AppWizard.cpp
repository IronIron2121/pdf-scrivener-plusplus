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
    wizard->end();
}

// this is mostly here to debug memory issues. it's not called anywhere
AppWizard::~AppWizard(){
    std::cout << "AppWizard destructor called" << std::endl;
}

// returns uSPaces
std::unordered_set<UChar32>* AppWizard::getUSpaces() {
    return &(this->uSpaces);
}
// returns uNewLines characters
std::unordered_set<UChar32>* AppWizard::getUNewLines() {
    return &(this->uNewLines);
}
// returns uPrintable characters
std::unordered_set<UChar32>* AppWizard::getUPrintable() {
    return &(this->uPrintable);
}
// return the whole list of printable characters
std::unordered_set<UChar32>* AppWizard::getUPrintablePlus() {
    return &(this->uPrintablePlus);
}

// function that takes a string and turns it into an unordered set of UChar32
void AppWizard::getSets(std::unordered_set<UChar32>& set, const std::string& stdStr) {
    icu::UnicodeString uStr = icu::UnicodeString::fromUTF8(stdStr);
    for (int32_t i = 0; i < uStr.length(); ++i) {
        set.insert(uStr.char32At(i));
    }
}

// returns a pointer to the list of character occurrences
std::map<UChar32, int>* AppWizard::getUCharOccurs() {
    return &(this->uCharOccurs);
}

// returns the number of occurrences of a specific character
int AppWizard::getUCharOccur(UChar32 thisBadChar){
    return this->uCharOccurs[thisBadChar];
}

// updates the char occurrences given a sent character
void AppWizard::upUCharOccur(UChar32 thisUChar) {
    this->uCharOccurs[thisUChar]++;
}

// increments the current bad character index
void AppWizard::upBIndex() {
    this->bIndex++;
    // std::cout << "bIndex: " << bIndex << std::endl;
}

UChar32 AppWizard::getCurrBadChar(){
    //std::cerr << "Error: bIndex exceeds uBadChars length. bIndex: " << bIndex << ", uBadChars length: " << uBadChars.length() << std::endl;
    return this->uBadChars[this->bIndex];
}


icu::UnicodeString AppWizard::getGivenBadChar(int index) {
    return this->uBadChars[index];
}

// function that checks if a character marks the end or beginning of a context
bool AppWizard::endChecker(UChar32 thisChar, const icu::UnicodeString& enders) {
    // if this character is an ender, return true
    return enders.indexOf(thisChar) != -1;
}

// function that finds the left and right limits of the context in which a bad character occurs
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

// function to get a string expressing the context in which a bad character occurs
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
// function to get a list of contexts in which a bad character occurs
std::vector<std::string> AppWizard::getBintexts() {
    // if bIndex is out of bounds, return an empty vector
    if (this->bIndex >= (this->uBadChars).length()) {
        std::cout << "bIndex out of bounds at getBintexts()" << std::endl;
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


// gets the character corresponding to the current bIndex and turns it into an std::string
std::string AppWizard::getDisplayChar() {
    // if there are no bad characters, return N/A
    if(!this->uBadChars.length()) {
        std::cout << "no bad characters" << std::endl;
        return "N/A";
    } else if (this->bIndex >= this->uBadChars.length()) {
        std::cout << "bIndex out of bounds at getDisplayChar()" << std::endl;
        return "N/A";
    }

    // grab the character
    UChar32 realBadChar = this->uBadChars[this->bIndex];
    // convert it to a unicode string
    icu::UnicodeString charToUString(realBadChar);
    // convert that to a std string, then return
    std::string uStringToStdString;
    charToUString.toUTF8String(uStringToStdString);
    return uStringToStdString;

}

// -- GET AND MODIFY FUNCTIONS FOR INITIAL AND PROCESSED PDF STRING -- //
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
// ------------------------------------------------------------- //

// -- GET AND MODIFY FUNCTIONS FOR INITIAL AND PROCESSED PDF-LIST -- //
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
// ------------------------------------------------------------------------- //

// get reference to replacement dictionary
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

int32_t* AppWizard::getCIndex(){
    return &(this->cIndex);
}