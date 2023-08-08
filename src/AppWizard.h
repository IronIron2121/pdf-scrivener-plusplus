#ifndef APPWIZARD_H
#define APPWIZARD_H

#include "FL/Fl_Window.H"
#include "FL/Fl_Wizard.H"
#include "OpenPDFPage.h"
#include "ChoicePage.h"
#include <vector>
#include <string>
#include <unordered_map>


class AppWizard : public Fl_Window {
private:
    Fl_Wizard* wizard;
    OpenPDFPage* openPage;
    ChoicePage* choicePage; 

    std::unordered_map<UChar32, int> uCharOccurs; // a pointer to the map of every char and its occurences

    std::vector<char> badCharsCh; // list of bad characters

    icu::UnicodeString uBadChars; // list of bad characters
    int bindex; // current bad character index

    icu::UnicodeString uPdfText; // initially extracted text as one long string
    icu::UnicodeString newPdfText; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString> uPdfList; // initially extracted text, page by page
    std::vector<icu::UnicodeString> newPdfList; // cleaned extracted text, page by page

    icu::UnicodeString uSpaces; // list of "good" characters
    icu::UnicodeString uPrintable; // list of "good" characters
    icu::UnicodeString uPrintablePlus; // uPrintable + extras
    icu::UnicodeString uNewLines; // list of new line characters

    std::unordered_map<std::string, ReplacementInfo> replacementDict; // replacement info for each bad char


    // combined structure to store and easily access replacement infos during replacement
    struct ReplacementInfo {
        bool contextual; // is this replacement context sensitive?
        std::string replacement; // what is the replacement?
    };

public:
    AppWizard(int w, int h, const char* title = 0);

    // getter methods
    int getBindex(); // returns current bad character index
    void upBindex(); // increment bindex
    std::string getDisplayChar();

    icu::UnicodeString getBadChar(); // gets the current bad character
    icu::UnicodeString getGivenBadChar(int index);
    std::string getConText(int indx, const icu::UnicodeString& pageText); // get context for given bindex

    icu::UnicodeString getUSpaces();
    icu::UnicodeString getUNewLines();
    icu::UnicodeString getUPrintable();
    icu::UnicodeString getUPrintablePlus();
    icu::UnicodeString getLocalPrintable();

    icu::UnicodeString* getUPdfText(); // gets the initial pdf text (single string)
    void pushToUPdfText(icu::UnicodeString pageText); // push to it
    icu::UnicodeString* getNewPdfText(); // gets the processed pdf text (single string)
    void pushToNewPdfText(icu::UnicodeString pageText); // push to it

    std::vector<icu::UnicodeString>* getUPdfList(); // gets the pdf pages (list of strings)
    void pushToUPdfList(icu::UnicodeString pageText); // push provided text to the book
    std::vector<icu::UnicodeString>* getNewPdfList(); // gets the pdf pages (list of strings)
    void pushToNewPdfList(icu::UnicodeString pageText); // push provided text to the book


    std::vector<std::string> getBintexts(); // a list of contexts for given bindex

    icu::UnicodeString* getUBadChars(); // return list of bad characters by reference

    std::unordered_map<UChar32, int>* getUCharOccurs(); // gets character occurences
    int getUCharOccur(UChar32 thisBadChar); // get the occurrences of a single character
    void upUCharOccur(UChar32 thisUChar); // increment the occurrence of a single character

    // utils
    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders); // check if a char is an ender
    void refreshVals(void* data); // refresh the values of the choice page when next is pressed
    std::pair<int32_t,int32_t> getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength); // get pointers for context
};

#endif // APPWIZARD_H
