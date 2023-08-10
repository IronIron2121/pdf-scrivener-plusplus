#ifndef OPENPDFPAGE_H
#define OPENPDFPAGE_H

#include "MyPage.h"

// gui stuff
#include "FL/Fl_Button.H"
#include "FL/Fl_Multiline_Output.H"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Wizard.H"

// pdf manipulation
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>

#include "ChoicePage.h"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>

class AppWizard; // Forward declaration

class OpenPDFPage : public MyPage {
private:
    Fl_Button* loadBtn; // load pdf button
    Fl_Multiline_Output* badOut; // viewer for bad characters
    Fl_Button* nextBtn; // go to next page button

    icu::UnicodeString* uPdfTextHere; // initially extracted text as one long string
    icu::UnicodeString* newPdfTextHere; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString>* uPdfListHere; // initially extracted text, page by page
    std::vector<icu::UnicodeString>* newPdfListHere; // cleaned extracted text, page by page

    ChoicePage** choicePageHere; // pointer to choice page

    icu::UnicodeString* uBadChars; // list of bad characters
    std::unordered_set<UChar32>* uSpaces; // list of space values
    std::unordered_set<UChar32>* uPrintable; // list of "good" characters
    std::unordered_set<UChar32>* uPrintablePlus; // good characters + a few extras
    std::unordered_set<UChar32>* uNewLines; // list of new line characters

    std::map<UChar32, int>* uCharOccurs; // a pointer to the map of every char and its occurences

    AppWizard* parent; // parent wizard

    void processPDFDoc(); // function to process pdfs
    void processPageText(const icu::UnicodeString& pageText); // process a single page of doc
    void processChar(UChar32 currentChar, bool& leadingWhiteSpace, int32_t charIt); // process a single char of page
    bool isPrintable(UChar32 currentChar); // check if char is printable / "good"
    void makeOutput(Fl_Multiline_Output* badHere); // print output to console
    
    void initAttributes();



public:
    OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title = 0);

    void loadPDFDoc(); // function to load pdfs
    static void goToChoicePage(Fl_Widget* w, void* data); // function to go to next page
    static void activateLoad(Fl_Widget* w, void* data); // trampoline


};
#endif