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
    void initAttributes(); // initialise attributes
    Fl_Button* loadBtn; // load pdf button
    Fl_Multiline_Output* badOut; // viewer for bad characters
    Fl_Button* nextBtn; // go to next page button

    icu::UnicodeString* uPdfTextHere; // initially extracted text as one long string
    icu::UnicodeString* newPdfTextHere; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString>* uPdfListHere; // initially extracted text, page by page
    std::vector<icu::UnicodeString>* newPdfListHere; // cleaned extracted text, page by page
    std::map<UChar32, int>* uCharOccursHere; // a pointer to the map of every char and its occurences
    icu::UnicodeString* uBadCharsHere; // list of bad characters
    std::unordered_set<UChar32>* uSpacesHere; // list of space values
    std::unordered_set<UChar32>* uPrintableHere; // list of "good" characters
    std::unordered_set<UChar32>* uPrintablePlusHere; // good characters + a few extras
    std::unordered_set<UChar32>* uNewLinesHere; // list of new line characters



    ChoicePage** choicePageHere; // pointer to choice page




    AppWizard* parent; // parent wizard

    void loadPDFDoc(); // load in a PDF doc
    void processPDFDoc(); // function to process the pdf
    void processPageText(const icu::UnicodeString& pageText); // process a single page of the pdf
    void processChar(UChar32 currentChar, bool& leadingWhiteSpace, int32_t charIt); // determine what type of char a char is, and what to do with it 
    bool isPrintable(UChar32 currentChar); // check if char is printable / "good"
    void makeOutput(Fl_Multiline_Output* badHere); // print bad characters to console
    

public:
    OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title = 0);

    static void goToChoicePage(Fl_Widget* w, void* data); // function to go to next page
    static void activateLoad(Fl_Widget* w, void* data); // trampoline


};
#endif