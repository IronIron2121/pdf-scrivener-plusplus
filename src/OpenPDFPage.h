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
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>


#include <iostream>
#include <sstream>
#include <unordered_map>

class AppWizard; // Forward declaration

class OpenPDFPage : public MyPage {
private:
    Fl_Button* loadBtn; // load pdf button
    Fl_Multiline_Output* badOut; // viewer for bad characters
    Fl_Button* nextBtn; // go to next page button

    // you get a static! you get a static! everyone gets a static!
    icu::UnicodeString* uPdfText; // initially extracted text as one long string
    icu::UnicodeString* newPdfText; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString>* uPdfList; // initially extracted text, page by page
    std::vector<icu::UnicodeString>* newPdfList; // cleaned extracted text, page by page

    icu::UnicodeString* uBadChars; // list of bad characters
    icu::UnicodeString uSpaces; // list of "bad" characters that have been uAccounted for
    icu::UnicodeString uPrintable; // list of "good" characters
    icu::UnicodeString uPrintablePlus; // uPrintable + extras
    icu::UnicodeString uNewLines; // list of new line characters

    std::unordered_map<UChar32, int>* uCharOccurs; // a pointer to the map of every char and its occurences

    AppWizard* parent; // parent wizard

    void processPDFDoc(); // function to process pdfs
    void processPageText(const icu::UnicodeString& pageText); // process a single page of doc
    void processChar(UChar32 currentChar, bool leadingWhiteSpace); // process a single char of page
    bool isPrintable(UChar32 currentChar); // check if char is printable / "good"
    void makeOutput(Fl_Multiline_Output* badHere); // print output to console

    static void activateLoad(Fl_Widget* w, void* data); // trampoline

public:
    OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title = 0);

    void loadPDFDoc(); // function to load pdfs
    static void goToChoicePage(Fl_Widget* w, void* data); // function to go to next page

};
#endif