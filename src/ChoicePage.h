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
    Fl_Box* thisCharLabel;
    std::vector<Fl_Box*> chartextBoxes;


    int32_t* bindexHere; // bad char index from parent

    icu::UnicodeString* uBadCharsHere; // list of bad characters from parent
    icu::UnicodeString* newPdfTextHere; // initially extracted text as one long string from parent
    std::vector<icu::UnicodeString>* newPdfListHere; 


    std::map<UChar32, int>* uCharOccursHere; // a pointer to the map of every char and its occurences from parent
    std::unordered_set<UChar32>* uPrintableHere;
    std::unordered_set<UChar32>* uNewLinesHere;
    UChar32 getCurrChar();


    // pointer to wizard dictionary
    // combined structure to store and easily access replacement infos during replacement
    std::string getDisplayChar();
    void doReplacements();
    ChoicePage* choicePage; 


    bool endChecker(UChar32 thisChar, const icu::UnicodeString& enders);
    std::pair<int32_t,int32_t> getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength);
    std::tuple<std::string, int, int, icu::UnicodeString> getConText(int indx, const icu::UnicodeString& pageText);
    std::vector<std::string> getBintexts();

    std::map<UChar32, ReplacementInfo>* replacementDictHere; // replacement info for each bad char
    
    Fl_Button* goodifyButton; // don't replace
    Fl_Button* replaceAllButton; // replace everything with input
    Fl_Input* replaceAllInput; // input for the above
    Fl_Button* contextButton; // replace based on context



    std::vector<Fl_Box*> getChartextBoxes(); // get context boxes for current character

    icu::UnicodeString justContext(int indx, const icu::UnicodeString& pageText);



    int numCharBoxes; // number of character boxes

    struct choicePack {
        int choice;
        ChoicePage* instance ;
    };

    void goodifyCb(); // replace bad char with itself
    void replaceAllCb(); // replace bad char with user input
    void contextCb(); // replace bad char contextually
    void nextChar(); // go-to next bad character

    // this object is a child of the prodigious AppWizard
    AppWizard* parent; 

public:
    ChoicePage(int x, int y, int w, int h,  AppWizard* parent, const char* title = 0);
    
    Fl_Box* getCharLabel(); // get string for current character

    void refreshVals(); // refresh display values
    static void activateChoiceClick(Fl_Widget* w, void* data); // go down decision path for user choice

};


#endif
