#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H

#include "MyPage.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <vector>
#include <string>
#include <unordered_map>




class AppWizard; // Forward declaration

class ChoicePage : public MyPage {
private:
    Fl_Box* thisCharLabel;
    std::vector<Fl_Box*> chartextBoxes;

    // combined structure to store and easily access replacement infos during replacement
    struct ReplacementInfo {
        bool contextual; // is this replacement context sensitive?
        std::string replacement; // what is the replacement?
    };
    static std::unordered_map<char, ReplacementInfo> replacementDict; // replacement info for each bad char
    
    Fl_Button* goodifyButton; // don't replace
    Fl_Button* replaceAllButton; // replace everything with input
    Fl_Input* replaceAllInput; // input for the above
    Fl_Button* contextButton; // replace based on context

    // this object is a child of the prodigious AppWizard
    AppWizard* parent; 

public:
    ChoicePage(int x, int y, int w, int h,  AppWizard* parent, const char* title = 0);
    
    Fl_Box* getCharLabel();
    std::vector<Fl_Box*> getChartextBoxes();

    static void goodifyCb(Fl_Widget* w, void* data); // replace bad char with itself
    static void replaceAllCb(Fl_Widget* w, void* data); // replace bad char with user input
    static void contextCb(Fl_Widget* w, void* data); // replace bad char contextually
    static void nextChar(Fl_Widget* w, void* data); // go-to next bad character
    static void doReplacements(); // run all replacements
};


#endif
