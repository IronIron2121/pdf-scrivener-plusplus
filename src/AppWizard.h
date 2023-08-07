#ifndef APPWIZARD_H
#define APPWIZARD_H

#include "FL/Fl_Window.H"
#include "FL/Fl_Wizard.H"
#include "OpenPDFPage.h"
#include "ChoicePage.h"
#include <vector>
#include <string>
#include <unordered_map>\

class AppWizard : public Fl_Window {
private:
    Fl_Wizard* wizard;
    OpenPDFPage* openPage;
    ChoicePage* choicePage;
    std::unordered_map<char, int> charOccur; // every char and its occurences
    std::string pdfText; // all of the pdf's initially extracted text
    std::string badChars = ""; // list of bad characters
    int bindex; // current bad character index

public:
    AppWizard(int w, int h, const char* title = 0);

    // getter methods
    int getBindex(); // returns current bad character index
    char getBadChar(); // gets the current bad character
    char getGivenBadChar(int index);
    std::string* getBadChars(); // gets the current bad character
    std::string* getPdfText(); // gets the pdf text
    
    std::unordered_map<char, int>* getCharOccur(); // gets character occurences
    std::string getConText(int indx, const std::string& pageText); // get context for given bindex
    std::vector<std::string> getBintexts(char thisBadChar); // a list of contexts for given bindex

    // utils
    int getCharOccurs(char thisBadChar);
    void upBindex(); // increment bindex
    void upCharOccur(char thisChar);
    bool endChecker(char thisChar, std::vector<char>& enders); // check if a char is an ender
    void refreshVals(void* data); // refresh the values of the choice page when next is pressed
};

#endif // APPWIZARD_H
