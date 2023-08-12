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
    int* cIndexHere;
    int listSize;

    Fl_Window* win;
    Fl_Scroll* scroll;


    Fl_Button* saveBtn;
    Fl_Button* noneBtn;

    Fl_Box* contextDisplay;
    Fl_Box* getContextDisplay();

    void nextContext();

    icu::UnicodeString getCurrentContext();

    Fl_Input* replacementInput;

    // variables referenced from parent
    std::string displayChar;
    icu::UnicodeString* newPdfTextHere; // cleaned extracted text as one long string

    std::vector<icu::UnicodeString>* newPdfListHere; // cleaned extracted text, page by page


    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* contextDictHere; // replacement info for each bad char
    std::map<UChar32, std::vector<icu::UnicodeString>>* contextListHere;


    icu::UnicodeString getConText(int32_t indx, const icu::UnicodeString& pageText);
    void getContexts(UChar32 thisChar);
    void drawThis();

    void deleteThis(std::map<icu::UnicodeString, ContextWidgets> contextWidgetsMap);

    Fl_Box* testLabel;

    void contextualise();
    
    icu::UnicodeString* uBadCharsHere; // list of bad characters
    std::unordered_set<UChar32>* uSpaces; // list of "bad" characters that have been uAccounted for
    std::unordered_set<UChar32>* uPrintable; // list of "good" characters
    std::unordered_set<UChar32>* uPrintablePlus; // uPrintable + extras
    std::unordered_set<UChar32>* uNewLines; // list of new line characters

    std::map<icu::UnicodeString, ContextWidgets> contextMap;
    std::map<UChar32, int>* uCharOccursHere; // a pointer to the map of every char and its occurrences


    AppWizard* parent; // parent wizard

    std::pair<int32_t, int32_t> getPointers(int32_t indx, const icu::UnicodeString& pageText, const int32_t thisPageLength);

    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders);

    std::string contextStr;





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
    void refreshContext();


};

#endif
