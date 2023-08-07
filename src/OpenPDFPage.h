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
    static std::string pageStr; // extracted text as string
    static std::string *badChars; // list of bad characters
    static std::string printable; // list of "good" characters
    static std::vector<std::string> pageList; // extracted text by page
    static std::unordered_map<char, int>* charOccur; // a pointer to the map of every char and its occurences

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