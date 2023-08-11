#ifndef CONTEXTPAGE_H
#define CONTEXTPAGE_H

#include "MyPage.h"

// GUI stuff
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Scroll.H>

// PDF manipulation
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>


#include <map>
#include <tuple>
#include <unordered_set>

class AppWizard; // Forward declaration
class Fl_Multiline_Output;

class ContextPage : public MyPage {
private:
    AppWizard* parent; // parent wizard

    void initAttributes();
    void initDisplays();
    void initPointers();
    // -------------- STRUCTS -------------- //
    struct ContextWidgets {
        Fl_Box* contextBox;
        Fl_Input* replacementInput;
        Fl_Button* saveBtn;
        Fl_Button* noneBtn;
        Fl_Button* undoBtn;
    };

    struct CbData {
        ContextPage* contextPage;
        int indx;
    };

    // -------------- NUMERICAL ATTRIBUTES & FUNCTIONS-------------- //

    int* bIndexHere; // bad char index from parent
    int* cIndexHere; // context index from parent
    int listSize; // size of current context list

    Fl_Box* contextDisplay; // show current context
    Fl_Button* saveBtn; // save input button
    Fl_Button* noneBtn; // don't replace button
    void nextContext(); // go to next context
    Fl_Input* replacementInput; // desired replacement input


    // variables referenced from parent
    icu::UnicodeString* newPdfTextHere; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString>* newPdfListHere; // cleaned extracted text, page by page
    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* contextDictHere; // replacement info for each bad char
    std::map<UChar32, std::vector<icu::UnicodeString>>* contextListHere; // list that maps each character to a simple list of contexts
    icu::UnicodeString* uBadCharsHere; // list of bad characters


public:
    ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title);
    void newInit();
    int x;
    int xGap;
    int y;
    int yGap;
    int w;
    int h;
    // make callbacks for all the buttons
    static void saveBtnCb(Fl_Widget* widget, void* data);
    static void noneBtnCb(Fl_Widget* widget, void* data);
    void refreshContext(); // refresh the context display


};

#endif
