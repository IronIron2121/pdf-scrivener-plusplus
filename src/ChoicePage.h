#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H

#include "MyPage.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <fstream>
#include <tuple>


#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>

#include "ReplacementInfo.h"

// forward declaration
class AppWizard; 
class ContextPage; 

class ChoicePage : public MyPage {
private:
    AppWizard* parent;


    struct choicePack {
        int choice;
        ChoicePage* instance ;
    };

    choicePack* pack0;
    choicePack* pack1;
    choicePack* pack2;



    int x;
    int xGap;
    int y;
    int yGap;
    int w;
    int h;

    int* bIndexHere; // bad char index from parent

    std::string displayChar;
    std::string charText;


    std::vector<Fl_Box*> charTextBoxes;

    icu::UnicodeString* uBadCharsHere; // list of bad characters from parent


    UChar32 currBadChar;
    ContextPage** contextPageHere;

    // combined structure to store and easily access replacement infos during replacement
    ChoicePage* choicePage; 

    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* contextDictHere;
    std::map<UChar32, ReplacementInfo>* replacementDictHere; // replacement info for each bad char


    Fl_Box* charLabel;
    Fl_Button* goodifyButton; // don't replace
    Fl_Button* replaceAllButton; // replace everything with input
    Fl_Button* contextButton; // replace based on context
    Fl_Input* replaceAllInput; // input for the replace all button

    int numCharBoxes; // number of character boxes


    void goodifyCb(); // replace bad char with itself
    void replaceAllCb(); // replace bad char with user input
    void contextCb(); // replace bad char contextually


public:
    ChoicePage(int x, int y, int w, int h,  AppWizard* parent, const char* title = 0);

    // initialize attributes, display widgets and pointers
    void initAttributes();
    void initDisplays();
    void initPointers();

    // refresh display values
    void refreshVals(); 
    void nextChar(); // go-to next bad character

    
    // trampoline for button callbacks
    static void activateChoiceClick(Fl_Widget* w, void* data); // go down decision path for user choice

};


#endif
