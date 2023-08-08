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
    icu::UnicodeString* uPdfStr; // extracted text as string
    std::vector<icu::UnicodeString> uPageList; // extracted text by page

    std::vector<icu::UnicodeString>* uBadChars; // list of bad characters
    icu::UnicodeString uAccounted; // list of "bad" characters that have been accounted for
    icu::UnicodeString uPrintable; // list of "good" characters
    icu::UnicodeString uPrintablePlus; // printable + extras
    std::unordered_map<icu::UnicodeString, int>* uCharOccur; // a pointer to the map of every char and its occurences

    AppWizard* parent; // parent wizard

    // this is a kind of care-package that contains data we need to access from inside static functions
    // we can't just use callback data because it doesn't accept enough args
    struct PassData {
        Fl_Multiline_Output* badOut; 
        AppWizard* parent;
    };


public:
    OpenPDFPage(int x, int y, int w, int h, AppWizard* parent, const char* title = 0);

    static void loadPDF(Fl_Widget* w, void* data); // function to load PDF
    static void goToChoicePage(Fl_Widget* w, void* data); // function to go to next page
};
#endif