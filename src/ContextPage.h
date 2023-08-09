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
    void contextualise();

    int x;
    int xGap;
    int y;
    int yGap;
    int w;
    int h;

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
    



    Fl_Window* win;
    Fl_Scroll* scroll;
    std::vector<Fl_Input*> inputs;
    std::vector<Fl_Button*> noneButtons;
    std::vector<Fl_Button*> undoButtons;
    std::vector<Fl_Button*> submitAllBtn;


    Fl_Button* saveBtn;
    Fl_Button* noneBtn;
    Fl_Button* undoBtn;
    Fl_Button* submitBtn;

    std::vector<Fl_Button*> saveBtns; // save replacement
    std::vector<Fl_Button*> noneBtns; // no replacement
    std::vector<Fl_Button*> undoBtns; // undo choice
    std::vector<Fl_Input*> replacementInputs; // input for the above

    std::vector<Fl_Box*> contextBoxes;

    Fl_Input* replacementInput;


    icu::UnicodeString* uPdfTextHere; // initially extracted text as one long string
    icu::UnicodeString* newPdfTextHere; // cleaned extracted text as one long string
    std::vector<icu::UnicodeString>* uPdfListHere; // initially extracted text, page by page
    std::vector<icu::UnicodeString>* newPdfListHere; // cleaned extracted text, page by page
    std::vector<icu::UnicodeString> contexts;

    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* contextDictHere; // replacement info for each bad char
    icu::UnicodeString getConText(int indx, const icu::UnicodeString& pageText);
    void getContexts(int thisChar);
    void drawThis();

    void deleteThis(std::map<icu::UnicodeString, ContextWidgets> contextWidgetsMap);


    
    icu::UnicodeString* uBadChars; // list of bad characters
    std::unordered_set<UChar32>* uSpaces; // list of "bad" characters that have been uAccounted for
    std::unordered_set<UChar32>* uPrintable; // list of "good" characters
    std::unordered_set<UChar32>* uPrintablePlus; // uPrintable + extras
    std::unordered_set<UChar32>* uNewLines; // list of new line characters

    std::map<UChar32, int>* uCharOccurs; // a pointer to the map of every char and its occurrences

    AppWizard* parent; // parent wizard

    std::pair<int32_t, int32_t> getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength);

    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders);



    void processPDFDoc(); // function to process pdfs
    void initAttributes();
    void processPageText(const icu::UnicodeString& pageText); // process a single page of doc
    void processChar(UChar32 currentChar, bool& leadingWhiteSpace, int32_t charIt); // process a single char of page
    bool isPrintable(UChar32 currentChar); // check if char is printable / "good"
    void makeOutput(Fl_Multiline_Output* badHere); // print output to console
    void processPage(const icu::UnicodeString& page);
    std::map<icu::UnicodeString, ContextWidgets> contextMap;



public:
    void newInit();
    ContextPage(int x, int y, int w, int h, AppWizard* parent, const char* title);

};

#endif
