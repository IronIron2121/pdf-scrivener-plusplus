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
    std::unordered_map<std::string, int> charOccur; // every char and its occurences
    std::string pdfText; // all of the pdf's initially extracted text
    std::vector <std::string> pdfPages; // all the pages
    std::vector<std::string> badChars; // list of bad characters
    std::vector<char> badCharsCh; // list of bad characters
    int bindex; // current bad character index
    static std::string printable; // printable + extras
    static std::string printablePlus; // printable + extras
    static std::string accounted; // printable + extras
    std::string localPrintable;


public:
    AppWizard(int w, int h, const char* title = 0);

    // getter methods
    int getBindex(); // returns current bad character index
    std::string getBadChar(); // gets the current bad character
    std::string getGivenBadChar(int index);
    std::vector<std::string>* getBadChars(); // gets the current bad character
    std::string* getPdfText(); // gets the pdf text
    std::vector<std::string>* getPdfPages(); // gets the pdf pages
    
    std::unordered_map<std::string, int>* getCharOccur(); // gets character occurences
    std::string getConText(int indx, const std::string& pageText); // get context for given bindex
    std::vector<std::string> getBintexts(); // a list of contexts for given bindex
    std::string getDisplayChar();
    std::string getLocalPrintable();


    // utils
    int getCharOccurs(std::string thisBadChar);
    void upBindex(); // increment bindex
    void upCharOccur(std::string thisChar);
    bool endChecker(char thisChar, std::vector<char>& enders); // check if a char is an ender
    void refreshVals(void* data); // refresh the values of the choice page when next is pressed
    void pushToPdfPages(std::string pageText); // gets the pdf pages
};

#endif // APPWIZARD_H
