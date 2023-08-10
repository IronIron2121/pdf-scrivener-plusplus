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
    OpenPDFPage* openPage;
    ChoicePage* choicePage; 
    ContextPage* contextPage;


    std::string spaces; // list of "good" characters
    std::string printable; // list of "good" characters
    std::string printablePlus; // uPrintable + extras
    std::string newLines; // list of new line characters

    icu::UnicodeString uBadChars; // list of bad characters
    icu::UnicodeString uPdfText; // initially extracted text as one long string
    icu::UnicodeString newPdfText; // cleaned extracted text as one long string

    std::vector<icu::UnicodeString> uPdfList; // initially extracted text, page by page
    std::vector<icu::UnicodeString> newPdfList; // cleaned extracted text, page by page
    std::vector<icu::UnicodeString> contextList;

    std::unordered_set<UChar32> uSpaces; // list of "good" characters
    std::unordered_set<UChar32> uPrintable; // list of "good" characters
    std::unordered_set<UChar32> uPrintablePlus; // uPrintable + extras
    std::unordered_set<UChar32> uNewLines; // list of new line characters

    std::map<UChar32, int> uCharOccurs; // a pointer to the map of every char and its occurences
    std::map<UChar32, ReplacementInfo> replacementDict; // replacement info for each bad char

public:
    AppWizard(int w, int h, const char* title = 0);

    int32_t bindex; // current bad character index


    // getter methods
    int32_t* getBindex(); // returns current bad character index
    void upBindex(); // increment bindex

    // get replacement dictionary
    std::map<UChar32, ReplacementInfo>* getReplacementDict(); 
    // initialise the check sets
    void getSets(std::unordered_set<UChar32>& set, const std::string& stdStr);

    ChoicePage** getChoicePage(); // get pointer to choice page
    ContextPage** getContextPage(); // get pointer to choice page

    std::string getDisplayChar(); // get the character to display on choice page

    UChar32 getBadChar(); // gets the current bad character
    std::string getConText(int indx, const icu::UnicodeString& pageText); // get context for given bindex

    std::unordered_set<UChar32>* getUSpaces();
    std::unordered_set<UChar32>* getUNewLines();
    std::unordered_set<UChar32>* getUPrintable();
    std::unordered_set<UChar32>* getUPrintablePlus();
    std::unordered_set<UChar32>* getLocalPrintable();

    icu::UnicodeString getGivenBadChar(int index); // gets bad char given index
    icu::UnicodeString* getUBadChars(); // return list of bad characters by reference

    icu::UnicodeString* getUPdfText(); // gets the initial pdf text (single string)
    icu::UnicodeString* getNewPdfText(); // gets the processed pdf text (single string)
    std::vector<icu::UnicodeString>* getUPdfList(); // gets the pdf pages (list of strings)
    std::vector<icu::UnicodeString>* getNewPdfList(); // gets the pdf pages (list of strings)
    std::vector<icu::UnicodeString>* getContextList();
    void pushToUPdfText(icu::UnicodeString pageText); // push to it
    void pushToUPdfList(icu::UnicodeString pageText); // push provided text to the book
    void pushToNewPdfText(icu::UnicodeString pageText); // push to it
    void pushToNewPdfList(icu::UnicodeString pageText); // push provided text to the book
    


    std::vector<std::string> getBintexts(); // a list of contexts for given bindex


    std::map<UChar32, int>* getUCharOccurs(); // gets character occurences
    int getUCharOccur(UChar32 thisBadChar); // get the occurrences of a single character
    void upUCharOccur(UChar32 thisUChar); // increment the occurrence of a single character

    // functions for character replacement
    // functions for replacement choices
    void goodifyRep();
    void replaceAllRep(UChar32 replacementChar);
    void contextualRep();
    void echoReplacement(); // debugging


    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>> contextDict;

    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* getContextDict();


    // utils
    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders); // check if a char is an ender
     // get pointers for context
    std::pair<int32_t,int32_t> getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength);
    void doReplacements();
};



#endif // APPWIZARD_H