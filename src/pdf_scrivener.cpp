// builtins
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <random>
#include <sstream>
#include <unordered_map>

// FLTK for GUI stuff
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Wizard.H>

// poppler for pdf parsing
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

// forward declaration for app container
class AppWizard;

// vector to contain extracted text from pdf
std::vector<std::string> pageList;
std::vector<std::string> convList; // after converting for display purposes
// character occurrences
std::unordered_map<char, int> charOccur;

// a string that will contain the whole pdf
std::string pdfText;

// the pdf we use for testing and filepath for directory
std::string pdfInputs = "bin/inputs/";
std::string pdfFileName = "GoodHeart";
std::string pdfFilePath = pdfInputs + pdfFileName + ".pdf";

// list of acceptable characters
std::string printable = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()_+-=[]{};':\\\",./<>?`~|\n";
// empty list of unacceptable characters
std::string badChars = "";


// together, these two functions help use replace bad characters with user input
struct ReplacementInfo {
    bool contextual; // is this replacment context-sensitive?
    std::string replacement; // what is the replacement character?
};
// map to store the above info for each bad character
std::unordered_map<char, ReplacementInfo> replacement_dict; // dict as a lover-letter to python



// generic page class. I should probably put this in a header file
// -------------------------------------------------------------------------- //
class MyPage : public Fl_Group {
public:
    MyPage(int x, int y, int w, int h, const char* title = 0) : Fl_Group(x, y, w, h, title) {}
};
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
class OpenPDFPage : public MyPage {
private:
    Fl_Button* loadBtn;
    Fl_Multiline_Output* badOut;
    Fl_Button* nextBtn;

public:
    OpenPDFPage(int x, int y, int w, int h, const char* title = 0) : MyPage(x, y, w, h, title) {
        
        // button to load, output to display bad characters, and button to go to next page
        loadBtn = new Fl_Button(50, 50, 150, 40, "Open PDF");
        badOut = new Fl_Multiline_Output(50, 200, 900, 600, "");
        nextBtn = new Fl_Button(250, 50, 150, 40, "Next");

        // activate buttons
        loadBtn->callback(loadPDF, badOut);
        nextBtn->callback(goToChoicePage);

        end();  
    }

    // load PDF, extract text, and find bad characters
    static void loadPDF(Fl_Widget* w, void* data) {
        const char* thisPDF = fl_file_chooser("Select a PDF", "*.pdf", NULL); // open file chooser 
        // if it's a valid file
        if (thisPDF) {
            // try to load it as a PDF
            std::cout << "Selected PDF: " << thisPDF << std::endl;
            std::unique_ptr<poppler::document> popplerDoc(poppler::document::load_from_file(thisPDF)); // load the PDF

            // if the loaded PDF is valid, then parse it and extract text
            if (popplerDoc) {
                std::cout << "Processing PDF" << std::endl;
                int numPages = (popplerDoc->pages()); // get the number of pages in the PDF
                std::cout << "Number of pages: " << numPages << std::endl;
                // for every page in the PDF
                for (int page_dex = 0; page_dex < numPages; page_dex++) {
                    std::cout << "Processing page " << page_dex << std::endl;
                    std::unique_ptr<poppler::page> pageIt(popplerDoc->create_page(page_dex)); // create a unique pointer to the page

                    // if the pointer is valid
                    if (pageIt) {
                        poppler::byte_array byteArray = pageIt->text().to_utf8(); // get data in utf8 encoding
                        std::string text(byteArray.begin(), byteArray.end()); // convert bytes to string
                        pageList.push_back(text); // push to the pageList vector
                    }
                    pageIt.reset(); // reset the page pointer
                }
                std::cout << "Finished Processing PDF" << std::endl;
                // for every page
                for(int pageInt = 0; pageInt < pageList.size(); pageInt++) {
                    std::cout << "Processing page " << pageInt << std::endl;
                    bool leadingWhiteSpace = true; // to skip leading whitespaces

                    // for every character in the page
                    for(int charIt = 0; charIt < pageList[pageInt].size(); charIt++) {
                        // initialise a string for the page
                        std::string pageText = "";
                        // grab the character
                        char thisChar = pageList[pageInt][charIt]; 


                        // if character is a newline, reset leading whitespace 
                        if(leadingWhiteSpace && thisChar == ' ') {
                            continue; // if it's a leading whitespace, skip it
                        } else if(thisChar == '\n'){
                            // if it's a newline, reset leading whitespace
                            leadingWhiteSpace = true;
                            // add it to the book string and the page string
                            pdfText += thisChar; 
                            pageText += thisChar;


                        } else {
                            // otherwise, it's not a leading whitespace
                            leadingWhiteSpace = false;
                            // we add it to the book string whether it's good or bad
                            pdfText += thisChar; 
                            pageText += thisChar;

                            // if it's not a printable (good) character
                            if(printable.find(thisChar) == std::string::npos) {
                                // if it's not already been added to the bad character list
                                if(badChars.find(thisChar) == std::string::npos) {
                                    badChars += thisChar; // add to bad character list
                                }
                                charOccur[thisChar]++; // add to number of this character's occurrences
                            } else {
                                // if it's a good character, just add to the number of this character's occurrences
                                charOccur[thisChar]++;
                            }
                        } 
                    }
                    
                }
                // create ostream to announce number of bad characters
                std::ostringstream os;
                os << "This document has " << badChars.size() << " bad characters:\n\n";

                // loop thru the charOccur map, append each bad character and its num occurrences
                for(int i = 0; i < badChars.size(); ++i) {
                    os << i << ' ' << badChars[i] << ", with: " << charOccur[badChars[i]] << " occurrences\n";
                }

                // container for multiline output
                Fl_Multiline_Output* badOut = (Fl_Multiline_Output*)data;

                // send bad character notification to the output
                badOut->value(os.str().c_str());
                badOut->redraw(); // redraw the output
                std::cout << os.str() << std::endl; // print to console
                std::cout << "Processing complete" << std::endl;
            } else{
                std::cout << "Invalid PDF" << std::endl;
            }
        } else {
            std::cout << "No PDF selected" << std::endl;
        }

        std::cout << "Finished Loading PDF" << std::endl;
    }
    static void goToChoicePage(Fl_Widget* w, void* data) {
        // tell the wizard to go to the next page
        ((Fl_Wizard*)w->parent()->parent())->next();
    }
};
// -------------------------------------------------------------------------- //

class ChoicePage : public MyPage {
private:
    Fl_Box* thisCharLabel;
    std::vector<Fl_Box*> chartextBoxes;

    Fl_Button* goodifyButton;
    Fl_Button* replaceAllButton;
    Fl_Input* replaceAllInput;
    Fl_Button* contextButton;

public:
    ChoicePage(int x, int y, int w, int h, const char* title = 0, AppWizard* parent = nullptr) : MyPage(x, y, w, h, title) {
        // Display current bad character
        std::string charText = "Current Character: " + std::string(1, parent->getBadChar());
        thisCharLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());

        // Display context for the bad character
        std::vector<std::string> listOfContexts = parent->getBintexts(parent->getBindex());
        // gap between each context
        int yGap = 50;
        // create a box for each context
        for (int context = 0; context < listOfContexts.size(); context++) {
            Fl_Box* box = new Fl_Box(x+10, y+yGap, w-20, 30, listOfContexts[context].c_str());
            chartextBoxes.push_back(box);
            yGap += 40;
        }

        // Buttons for actions
        goodifyButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Do not replace this character");
        goodifyButton->callback(goodifyCb, parent);

        yGap += 50;
        replaceAllButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Replace all instances of this character with input below");
        replaceAllInput = new Fl_Input(x+10, y+yGap+25, w-20, 40, "Replacement: ");
        replaceAllButton->callback(replaceAllCb, parent);

        yGap += 50;
        contextButton = new Fl_Button(x+10, y+yGap, w-20, 40, "Choose a different replacement dependent on character context");
        contextButton->callback(contextCb, parent);

        end();
    }

    // get the chartext label
    Fl_Box* getCharLabel() {
        return thisCharLabel;
    }

    // get character context boxes
    std::vector<Fl_Box*> getChartextBoxes() {
        return chartextBoxes;
    }

    static void goodifyCb(Fl_Widget* w, void* data) {
        // show em who's daddy
        AppWizard* parent = (AppWizard*)data;
        // make this character's replacement itself, declare it as non-contextual
        replacement_dict[parent->getBadChar()].replacement = parent->getBadChar();
        replacement_dict[parent->getBadChar()].contextual = false;
        nextChar(w, data);
    }

    static void replaceAllCb(Fl_Widget* w, void* data) {
        // show em who's daddy
        AppWizard* parent = (AppWizard*)data;
        ChoicePage* thisPage = (ChoicePage*)w->parent();
        // replace every instance of this character with the user's input
        replacement_dict[parent->getBadChar()].replacement = thisPage->replaceAllInput->value();
        replacement_dict[parent->getBadChar()].contextual = false;
    }

    static void contextCb(Fl_Widget* w, void* data) {
        AppWizard* parent = (AppWizard*)data;
    }

    static void nextChar(Fl_Widget* w, void* data) {
        // show em who's daddy (yes, it's that funny)
        AppWizard* parent = (AppWizard*)data;
        // increment the bad character index
        parent->upBindex();
        // if the bad character index is greater than the number of bad characters
        if(parent->getBindex() >= badChars.size()) {
            // start replacing the bad characters
            doReplacements();
        } else {
            // otherwise refresh the page values
            parent->refreshVals(data); 
        }
    }

    static void doReplacements(){

    }
};

// -------------------------------------------------------------------------- //
class AppWizard : public Fl_Window {
private:
    Fl_Wizard* wizard;
    OpenPDFPage* openPage;
    ChoicePage* choicePage;
    int bindex;

public:
    AppWizard(int w, int h, const char* title = 0) : Fl_Window(w, h, title){
        wizard = new Fl_Wizard(0, 0, w, h);
        openPage = new OpenPDFPage(0, 0, w, h, "PDF Scrivener - Open PDF");
        choicePage = new ChoicePage(0, 0, w, h, "PDF Scrivener - Choice Page", this);

        // add the pages to the wizard
        wizard->add(openPage);
        wizard->add(choicePage);

        // hide all pages except the first
        wizard->value(openPage);

        // integer to store which bad character we're on
        int bindex = 0;
        
        // end the wizard
        wizard->end();

    }

    // return current bad character index
    int getBindex() {
        return bindex;
    }

    // iterate current bad character index by 1
    void upBindex() {
        this->bindex++;
    }

    // return current bad character
    char getBadChar() {
        return badChars[bindex];
    }


    std::vector<std::string> getConText(int indx, std::string& pageText) {
        // initialise vector for context of bad characters
        std::vector<std::string> conText;

        // characters that end a sentence
        std::vector<char> enders = {' ', '.', '\n'};

        // init context span indices
        int leftPointer = indx;
        int rightPointer = indx;

        // find the leftmost limit of the context
        while(!endChecker(pageText[leftPointer], enders) && leftPointer > 0) {
            leftPointer--;
        }

        // find the rightmost limit of the context
        while(!endChecker(pageText[rightPointer], enders) && rightPointer < pageText.size()) {
            rightPointer++;
        }

        // add the context to the vector and return it
        std::string context = pageText.substr(leftPointer, rightPointer - leftPointer);
        return conText;
    }

    // array that gets the context of a provided bad character
    std::vector<std::string> getBintexts(char thisBadChar){
        // get whichever is smaller - number of char occurences, or 3
        int numExamples = std::min(charOccur[thisBadChar], 3);

        // vars to store the indices of the bad characters
        std::vector<int> indices; // where we'll store character indices
        std::string outString; // where we'll store context
        int minDex = 0; // start from 0

        for(int i = 0; i < numExamples; i++) {
            // get the index of the next occurrence of the bad character
            int thisDex = pdfText.find(thisBadChar, minDex);
            // push it to the vector
            indices.push_back(thisDex);
            // set the minimum index to the next index 
            minDex = thisDex + 1;
        }

        // get the context of each bad character
        std::vector<std::string>contextList;
        for(int i = 0; i < indices.size(); i++) {
            contextList.push_back(getConText(indices[i], pdfText)[0]);
        }

        // return a vector containing the index of the bad character
        return contextList;

    }


    // checks if a character is a sentence-ender
    bool endChecker(char thisChar, std::vector<char>& enders) {
        // if this character is an ender, return true
        return std::find(enders.begin(), enders.end(), thisChar) != enders.end();
    }

    void refreshVals(void* data) {
        // update bad char display
        std::string newText = "Current Character: " + std::string(1, this->getBadChar());
        this->choicePage->getCharLabel()->copy_label(newText.c_str());

        std::vector<Fl_Box*> chartextBoxes = this->choicePage->getChartextBoxes();

        // update contexts display
        std::vector<std::string> listOfContexts = this->getBintexts(this->bindex);
        for (int i = 0; i < listOfContexts.size() && i < chartextBoxes.size(); i++) {
            chartextBoxes[i]->copy_label(listOfContexts[i].c_str());
        }
    }
};

// -------------------------------------------------------------------------- //
int main(int argc, char** argv) {
    AppWizard appWin(1000, 800, "PDF Scrivener");
    appWin.show(argc, argv);
    return Fl::run();
}
// -------------------------------------------------------------------------- //
