#include "ChoicePage.h"
#include "AppWizard.h"
#include "ContextPage.h"
#include <iostream>


ChoicePage::ChoicePage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // init parent and parameter attributes
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;

    // init other attributes and pointers
    this->initPointers();
    this->initAttributes();
    this->initDisplays();

    this->end();
}

void ChoicePage::initAttributes(){
    // packs of attributes for button callbacks
    this->pack0 = new choicePack;
    this->pack0->choice = 0;
    this->pack0->instance = this;

    this->pack1 = new choicePack;
    this->pack1->choice = 1;
    this->pack1->instance = this;

    this->pack2 = new choicePack;
    this->pack2->choice = 2;
    this->pack2->instance = this;

    // y-increment gap between each button
    this->yGap = 100;
}

void ChoicePage::initPointers(){
    this->bIndexHere = this->parent->getCIndex();
    this->uBadCharsHere = this->parent->getUBadChars();
    this->replacementDictHere = this->parent->getReplacementDict();
    this->uCharOccursHere = this->parent->getUCharOccurs();
    this->newPdfTextHere = this->parent->getNewPdfText();
    this->newPdfListHere = this->parent->getNewPdfList();
    this->uPrintableHere = this->parent->getUPrintable();
    this->uNewLinesHere = this->parent->getUNewLines();
    this->contextPageHere = this->parent->getContextPage();
    this->contextDictHere = this->parent->getContextDict();
}

void ChoicePage::initDisplays(){

    // Display current bad character
    this->displayChar = "N/A";
    this->charText = "Current Character: ";
    this->thisCharLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());


    // create a box for each context
    for (int contextIt = 0; contextIt < 3; contextIt++) {
        this->y = y+yGap;
        Fl_Box* box = new Fl_Box(this->x+10, this->y, this->w, 70);
        this->charTextBoxes.push_back(box);
    }


    // Buttons for actions
    // button to replace all instances of this character with itself
    this->goodifyButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Do not replace this character");
    this->goodifyButton->callback(this->activateChoiceClick, this->pack0);

    this->y = this->y + this->yGap; // increment y

    // button to replace all instances of this character with given input
    this->replaceAllButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Replace all instances of this character with input below");
    this->replaceAllInput = new Fl_Input(this->x+10, this->y+25, this->w-20, 40, "Replacement: ");
    this->replaceAllButton->callback(this->activateChoiceClick, this->pack1);

    this->y = this->y + this->yGap; // increment y

    // button to replace this character with a different character depending on context it appears in
    this->contextButton = new Fl_Button(this->x+10, this->y, this->w-20, 40, "Choose a different replacement dependent on character context");
    this->contextButton->callback(this->activateChoiceClick, this->pack2);

}

void ChoicePage::activateChoiceClick(Fl_Widget* w, void* data){
    // get the passed pack, and unpack it
    choicePack* pack = static_cast<choicePack*>(data);
    int choice = pack->choice;
    ChoicePage* thisPage = pack->instance;

    // get the current bad character
    thisPage->currBadChar = thisPage->parent->getCurrBadChar();

    // trigger event based on choices
    if(choice == 0){
        thisPage->goodifyCb();
    } else if(choice == 1){
        thisPage->replaceAllCb();
    } else if(choice == 2){
        thisPage->contextCb();
    }
}

void ChoicePage::goodifyCb() { 
    // non-contextual replacement 
    (*(this->replacementDictHere))[this->currBadChar].contextual = false;

    // replacement is the character itself, converted from uChar to uString
    icu::UnicodeString thisRep = icu::UnicodeString(this->currBadChar);
    (*(this->replacementDictHere))[this->currBadChar].replacement = thisRep; 

    nextChar(); // go to next character
}


void ChoicePage::replaceAllCb() {
    // non-contextual replacement
    (*(this->replacementDictHere))[this->currBadChar].contextual = false;

    // replace every instance of this character with the user's input
    std::string inputStr = this->replaceAllInput->value();
    icu::UnicodeString inputU(inputStr.c_str()); // convert input to unicode string

    // put it in the replacement dictionary
    (*(this->replacementDictHere))[this->currBadChar].replacement = inputU;
    
    nextChar(); // go to next character

}

void ChoicePage::contextCb() {
    std::cout << "BINDEX ADDRESS IN CHOICE == " << &bIndexHere << std::endl;
    this->hide();
    (**contextPageHere).show();
    std::cout << "PARENT ADDRESS IN CHOICE == " << &parent << std::endl;
    (**contextPageHere).newInit(&parent);
}

void ChoicePage::nextChar() {
    // increment the bad character index
    this->parent->upBIndex();
    // std::cout << "\nbIndexHere Next Char:" << *bIndexHere << std::endl;

    // if the bad character index is greater than the number of bad characters
    if(*(this->bIndexHere) >= (*(this->uBadCharsHere)).length()) {
        std::cout << "Going for replacements!" << std::endl;
        // start replacing the bad characters
        this->doReplacements();
    } else {
        // otherwise refresh the page values
        this->refreshVals(); 
    }
}

// update the values on page
void ChoicePage::refreshVals() {
    // update bad char display
    std::string newText = "Current Character: " + std::string(this->parent->getDisplayChar());
    std::cout << "new text: " << newText << std::endl;
    this->thisCharLabel->copy_label(newText.c_str());

    // get a list of contexts for the current bad character
    std::vector<std::string> listOfContexts = this->parent->getBintexts();

    // get the size of the list and the boxes    
    int listSize = listOfContexts.size();
    int boxSize = charTextBoxes.size();

    // update contexts display
    for (int i = 0; i < listOfContexts.size(); i++) {
        //std::cout << "copying label at index " << i << std::endl;
        //std::cout << "label: " << listOfContexts[i].c_str() << std::endl;
        this->charTextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
    // if there's an empty box, fill it with N/A
    if (listSize < boxSize) {
        for (int i = listSize; i < boxSize; i++) {
            this->charTextBoxes[i]->copy_label("N/A");
        }
    }
}

bool ChoicePage::endChecker(UChar32 thisChar, const icu::UnicodeString& enders) {
    // if this character is an ender, return true
    return enders.indexOf(thisChar) != -1;
}
std::pair<int32_t,int32_t> ChoicePage::getPointers(int indx, const icu::UnicodeString& pageText, const int32_t thisPageLength){
    // characters that end a sentence
    icu::UnicodeString enders = u" .,\n";
    // init context span indices
    int32_t leftPointer = indx;
    int32_t rightPointer = indx;
    // find the leftmost limit of the context
    while(!endChecker(pageText.charAt(leftPointer-1), enders) && leftPointer > 0) {
        leftPointer--;
    }
    // find the rightmost limit of the context
    while(!endChecker(pageText[rightPointer], enders) && rightPointer < thisPageLength-1) {
        rightPointer++;
    }

    // make them into a pair and return
    std::pair<int32_t,int32_t> this_out = std::make_pair(leftPointer, rightPointer);
    return this_out;
}
// just returns the context
icu::UnicodeString ChoicePage::justContext(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    // error handling
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return icu::UnicodeString("");
    }

    // get the pointers for this run and assign them
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;

    // get the relative position of the bad character
    int32_t leftDiff = indx - leftPointer;

    // extract the context 
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);

    return context;
}

std::vector<std::string> ChoicePage::getBintexts() {
    // if bIndex is out of bounds, return an empty vector
    if (*(this->bIndexHere) >= (*(this->uBadCharsHere)).length()) {
        std::cout << "bIndex out of bounds at getBintexts()" << std::endl;
        return {};
    } else{
        // get whichever is number is smaller - number of char occurences, or 3
        int numExamples = std::min((*(this->uCharOccursHere))[this->currBadChar], 3);

        // var to store the indices of the bad characters
        std::vector<int32_t> indices;

        // try to .find() the bad character in the pdfText - starting from char 0
        int32_t minDex = 0;

        // for every example we wanna make
        for(int i = 0; i < numExamples; i++) {
            // try to find the next occurrence of this character
            int32_t thisDex = (*(this->newPdfTextHere)).indexOf(this->currBadChar, minDex);

            // if we found it, add it to the list of indices
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::tuple<std::string, int, int, icu::UnicodeString> contOut= getConText(indices[i], (*(this->newPdfTextHere)));
            std::string thisContext = std::get<0>(contOut);
            int thisPos = std::get<1>(contOut);
            int thisLength = std::get<2>(contOut);

            // create an underline the same length as the context
            std::string underLine(thisLength, '_');
            // turn position corresponding to bad character into an arrow
            underLine[thisPos] = '^';
            // add the underline to the context, then push context to list of contexts
            thisContext += "\n" + underLine;
            contextList.push_back(thisContext);
        }

        // return a vector containing the index of the bad character
        return contextList;
    }
}


void ChoicePage::doReplacements(){
    // Open a .txt file to write everything to
    // TODO - ADD PDF NAME HERE
    std::ofstream outFile("outputAll.txt");

    // catch opening errors
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output.txt for writing." << std::endl;
        return;
    }

    int i = 0;
    // Go through the book page by page and get replacements by searching in the map
    for (const auto& pageText : *newPdfListHere) {
        i++;
        // a blank page to copy to
        icu::UnicodeString modText = icu::UnicodeString::fromUTF8("");
        
        // For every character in the page
        for (int32_t charIndex = 0; charIndex < pageText.length(); ) {
            // Grab the character
            UChar32 thisChar = pageText.char32At(charIndex);

            // if this char is printable
            if ((*uPrintableHere).find(thisChar) != (*uPrintableHere).end()) {
                // simply copy over and move over by its size
                modText += thisChar;
                charIndex += U16_LENGTH(thisChar);  // Move by the length of the character
            } else if ((*uNewLinesHere).find(thisChar) != (*uNewLinesHere).end()) {
                // if it's a newline-like character, just add a newline
                UChar32 replacement = u'\n';
                modText += replacement;
                charIndex += U16_LENGTH(thisChar);  // move over by newline length
                
            } else {
                // otherwise it's a bad character
                // if it's non-contextual, replace it with its replacement
                if (!((*replacementDictHere)[thisChar].contextual)) {
                    // get the replacement and convert it to a UChar32
                    icu::UnicodeString replacement = (*replacementDictHere)[thisChar].replacement;
                    modText += replacement;
                    charIndex += U16_LENGTH(thisChar);  // you know the drill
                } else{
                    // if it's contextual, find its context and provide the suitable replacement
                    // get conText tuple
                    std::tuple<std::string, int, int, icu::UnicodeString> conText = getConText(charIndex, pageText);
                    // just grab the context
                    icu::UnicodeString context = std::get<3>(conText);
                    // TODO: index into the context dictionary

                    icu::UnicodeString replacement = (*contextDictHere)[thisChar][context];
                    modText += replacement;
                    charIndex += U16_LENGTH(thisChar);  // iterate past the character
                }

                /*
                icu::UnicodeString original = icu::UnicodeString(thisChar);
                std::string originalHolder;
                original.toUTF8String(originalHolder);
                std::string replacementHolder;
                replacement.toUTF8String(replacementHolder);
                std::cout << "replaced " << originalHolder << " with " << replacementHolder << std::endl;
                */
            }
        }
        
        // Convert the page to UTF8 and write to the file
        std::string utf8Page;
        modText.toUTF8String(utf8Page);
        outFile << utf8Page;
    }

    std::cout << "Done!" << std::endl;

    // Close the file
    outFile.close();

}