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
    ContextPage** contextPageHere;
    ChoicePage* choicePage; 

    // struct containing vars to send to callbacks
    struct choicePack {
        int choice;
        ChoicePage* instance ;
    };

    // choice packs for each option
    choicePack* pack0;
    choicePack* pack1;
    choicePack* pack2;

    // basic object attributes
    int x;
    int xGap;
    int y;
    int yGap;
    int w;
    int h;

    int* bIndexHere; // bad char index from parent
    icu::UnicodeString* uBadCharsHere; // list of bad characters from parent
    std::map<UChar32, std::map<icu::UnicodeString, icu::UnicodeString>>* contextDictHere;

    std::string displayChar; // current displayed character
    std::string charText; // string expressing current displayed character
    std::vector<Fl_Box*> charTextBoxes; // boxes to display current character contexts

    UChar32 currBadChar; // store current bad character



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

    void refreshVals(); // refresh values on page
    void nextChar(); // go-to next bad character

    
    // trampoline for button callbacks
    static void activateChoiceClick(Fl_Widget* w, void* data); // go down decision path for user choice

};


#endif
