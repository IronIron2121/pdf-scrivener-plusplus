#ifndef APPWIZARD_H
#define APPWIZARD_H


#include "FL/Fl_Window.H"
#include "ReplacementInfo.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <map>

// forward declarations
class Fl_Wizard;
class OpenPDFPage;
class ChoicePage;
class ContextPage;


class AppWizard : public Fl_Window {
private:
    Fl_Wizard* wizard;


    std::string pdfName; // store the name of the pdf
    std::string spaces; // list of "good" characters
    std::string printable; // list of "good" characters
    std::string printablePlus; // uPrintable + extras
    std::string newLines; // list of new line characters

    icu::UnicodeString uBadChars; // list of bad characters
    icu::UnicodeString uPdfText; // initially extracted text as one long string
    icu::UnicodeString newPdfText; // cleaned extracted text as one long string

    std::vector<icu::UnicodeString> uPdfList; // initially extracted text, page by page
    std::vector<icu::UnicodeString> newPdfList; // cleaned extracted text, page by page
    std::map<UChar32, std::vector<icu::UnicodeString>> contextList;

    std::unordered_set<UChar32> uSpaces; // list of "good" characters
    std::unordered_set<UChar32> uPrintable; // list of "good" characters
    std::unordered_set<UChar32> uPrintablePlus; // uPrintable + extras
    std::unordered_set<UChar32> uNewLines; // list of new line characters

    std::map<UChar32, int> uCharOccurs; // a pointer to the map of every char and its occurences
    std::map<UChar32, ReplacementInfo> replacementDict; // replacement info for each bad char

public:
    AppWizard(int w, int h, const char* title = 0);

    OpenPDFPage* openPage; // page to open and load a pdf
    ChoicePage* choicePage; // page to choose what you want to do with each character
    ContextPage* contextPage; // page to choose what you want to do with each context for a character

    ChoicePage** getChoicePage(); // get pointer to choice page
    ContextPage** getContextPage(); // get pointer to choice page

    int bIndex; // current bad character index
    int* getBIndex(); // returns cIndex
    void upBIndex(); // increment bIndex

    int cIndex; // current context index
    int* getCIndex(); 
    void upCIndex(); 
    void resetCIndex(); // reset cindex after context page for one character is finished

    // get replacement dictionary
    std::map<UChar32, ReplacementInfo>* getReplacementDict(); 


    std::string getDisplayChar(); // get the character to display on choice page

    UChar32 getCurrBadChar(); // gets the current bad character

    // functions to retrieve  our character sets
    void getSets(std::unordered_set<UChar32>& set, const std::string& stdStr); // create a set from a string
    std::unordered_set<UChar32>* getUSpaces(); // set of spaces
    std::unordered_set<UChar32>* getUNewLines(); // set of new-line-likes
    std::unordered_set<UChar32>* getUPrintable(); // set of standard printabecharacters
    std::unordered_set<UChar32>* getUPrintablePlus(); // all of the above in on set

    // functions to retrieve bad characters
    icu::UnicodeString getGivenBadChar(int index); // gets bad char given a specific index
    icu::UnicodeString* getUBadChars(); // returns whole list of bad characters

    // functions to and access retrieve our pdf texts
    std::string* getPdfName(); // return the title of the pdf
    void setPdfName(std::string givenName); // set the title of the pdf
    icu::UnicodeString* getUPdfText(); // gets the initial pdf text (single string)
    icu::UnicodeString* getNewPdfText(); // gets the processed pdf text (single string)
    std::vector<icu::UnicodeString>* getUPdfList(); // gets the initial pdf text (list of strings)
    std::vector<icu::UnicodeString>* getNewPdfList(); // gets the processed pdf text (list of strings)
    std::map<UChar32, std::vector<icu::UnicodeString>>* getContextList();
    void pushToUPdfText(icu::UnicodeString pageText); // push text to pdf string
    void pushToNewPdfText(icu::UnicodeString pageText); 
    void pushToUPdfList(icu::UnicodeString pageText); // push text to pdf list
    void pushToNewPdfList(icu::UnicodeString pageText); 


    // functions to retrieve and access the contexts in which our bad characters occur
    // get a message expressing the context in which a character occurs, its relative position within the context, the context length, and the context alone
    std::tuple<std::string, int, int, icu::UnicodeString> getConText(int indx, const icu::UnicodeString& pageText); 
    icu::UnicodeString getConTextC(int indx, const icu::UnicodeString& pageText); // simplified version of above. get a single context for character at given index
    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>> contextDict; // dictionary containing bad character contexts and associated replacements 
    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* getContextDict(); // retrieve context dictionary
    std::vector<std::string> getListOfContexts(); // function that returns a list of contexts for current bad character
    icu::UnicodeString getCurrentContext(); // return the current context in context page
    void getContextsForRep(UChar32 thisChar); // get all contexts for a given character

    // functions to retrieve and access the occurrences of our bad characters
    std::map<UChar32, int>* getUCharOccurs(); // return list of bad characters and their num occurrences
    int getUCharOccur(UChar32 thisBadChar); // get the occurrences for a single bad character
    void upUCharOccur(UChar32 thisUChar); // increment the occurrences of a single bad character

    // functions for character replacement choices
    void goodifyRep(); // replace a character with itself
    void replaceAllRep(UChar32 replacementChar); // replace all instances of a character with input
    void contextualRep(); // replace different instances of a character differently depending on context


    // utils

    // check if a character marks the end or beginning of a context
    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders); 
     // get the leftmost and rightmost limits of the context in which a bad character occurs
    std::pair<int32_t,int32_t> getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength);
    // replace every bad character with its given replacement
    void doReplacements();

    ~AppWizard();
};



#endif // APPWIZARD_H