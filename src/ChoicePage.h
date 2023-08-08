#ifndef CHOICEPAGE_H
#define CHOICEPAGE_H

#include "MyPage.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

class AppWizard; // Forward declaration

class ChoicePage : public MyPage {
private:
    Fl_Box* thisCharLabel;
    std::vector<Fl_Box*> chartextBoxes;

    std::string finalString; // final string to be outputted

    Fl_Button* goodifyButton; // don't replace
    Fl_Button* replaceAllButton; // replace everything with input
    Fl_Input* replaceAllInput; // input for the above
    Fl_Button* contextButton; // replace based on context

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
    
    Fl_Box* getCharLabel();
    std::vector<Fl_Box*> getChartextBoxes();

    static void activateChoiceClick(Fl_Widget* w, void* data);
};


#endif
