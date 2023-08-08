#include "ChoicePage.h"
#include "AppWizard.h"

ChoicePage::ChoicePage(int x, int y, int w, int h, AppWizard* parent, const char* title) : MyPage(x, y, w, h, title) {
    // Display current bad character
    std::string displayChar = parent->getDisplayChar();
    std::string charText = "Current Character: " + displayChar;
    thisCharLabel = new Fl_Box(x+10, y+10, w-20, 30, charText.c_str());


    // Display context for the bad character
    std::vector<std::string> listOfContexts = parent->getBintexts();

    bindexHere = parent->getBindex();
    uBadCharsHere = parent->getUBadChars();
    replacementDictHere = parent->getReplacementDict();


    finalString = "";
    
    // gap between each context
    int yGap = 100;
    
    // create a box for each context
    for (int context = 0; context < 3; context++) {
        y = y+yGap;
        Fl_Box* box = new Fl_Box(x+10, y, w, 70);
        chartextBoxes.push_back(box);
    }

    y = y+yGap;

    choicePack* pack0 = new choicePack;
    pack0->choice = 0;
    pack0->instance = this;
    choicePack* pack1 = new choicePack;
    pack1->choice = 1;
    pack1->instance = this;
    choicePack* pack2 = new choicePack;
    pack2->choice = 2;
    pack2->instance = this;

    // Buttons for actions
    goodifyButton = new Fl_Button(x+10, y, w-20, 40, "Do not replace this character");
    goodifyButton->callback(activateChoiceClick, pack0);

    y = y+yGap;
    replaceAllButton = new Fl_Button(x+10, y, w-20, 40, "Replace all instances of this character with input below");
    replaceAllInput = new Fl_Input(x+10, y+25, w-20, 40, "Replacement: ");
    replaceAllButton->callback(activateChoiceClick, pack1);\

    y = y+yGap;
    contextButton = new Fl_Button(x+10, y, w-20, 40, "Choose a different replacement dependent on character context");
    contextButton->callback(activateChoiceClick, pack2);

    end();
}


Fl_Box* ChoicePage::getCharLabel() {
    return thisCharLabel;
}

std::vector<Fl_Box*> ChoicePage::getChartextBoxes() {
    return chartextBoxes;
}

void ChoicePage::activateChoiceClick(Fl_Widget* w, void* data){
    // get the passed pack, and unpack it
    choicePack* pack = static_cast<choicePack*>(data);
    int choice = pack->choice;
    ChoicePage* thisPage = pack->instance;

    // trigger event based on choices
    if(choice == 0){
        thisPage->goodifyCb();
    } else if(choice == 1){
        thisPage->replaceAllCb();
    } else if(choice == 2){
        thisPage->contextCb();

    }
}

std::string ChoicePage::getDisplayChar() {
    // get the bad character
    UChar32 badChar = (*uBadCharsHere)[*bindexHere];

    // convert it to a unicode string
    icu::UnicodeString badCharU(badChar);

    // convert that to a std::string
    std::string badCharStr;
    badCharU.toUTF8String(badCharStr);

    // return the std::string
    return badCharStr;
}

void ChoicePage::goodifyCb() { 
    // make this character's replacement itself, declare it as non-contextual
    std::cout << "bindexHere: " << *bindexHere << std::endl;
    
    // i apologise sincerely for this. there was no other way
    // non-contextual replacement
    ((*replacementDictHere)[*bindexHere]).contextual = false;

    // replacement is the character itself
    std::string repStr = getDisplayChar();
    ((*replacementDictHere)[*bindexHere]).replacement = repStr;
    nextChar();
}


void ChoicePage::replaceAllCb() {
    // replace every instance of this character with the user's input
    this->parent->replaceAllRep(this->replaceAllInput->value()[0]);
    this->parent->echoReplacement();
}

void ChoicePage::contextCb() {
    // TODO
}

void ChoicePage::nextChar() {
    // increment the bad character index
    (*bindexHere)++;
    std::cout << "bindexHere Next Char: " << *bindexHere << std::endl;

    // if the bad character index is greater than the number of bad characters
    if(*bindexHere >= (*uBadCharsHere).length()) {
        std::cout << "Going for replacements!" << std::endl;
        // start replacing the bad characters
        doReplacements();
    } else {
        // otherwise refresh the page values
        refreshVals(); 
    }
}

void ChoicePage::doReplacements(){

}

void ChoicePage::refreshVals() {
    // update bad char display
    std::string newText = "Current Character: " + std::string(this->getDisplayChar());
    std::cout << "new text: " << newText << std::endl;

    Fl_Box *thisCharLabel = getCharLabel();
    thisCharLabel->copy_label(newText.c_str());


    // grab the character context boxes
    std::vector<Fl_Box*> chartextBoxes = getChartextBoxes();

    // get the contexts for the current bad character
    std::vector<std::string> listOfContexts = parent->getBintexts();

    int listSize = listOfContexts.size();
    int boxSize = chartextBoxes.size();
    int listBoxRange = listSize - boxSize;

    // update contexts display
    for (int i = 0; i < listOfContexts.size(); i++) {
        std::cout << "copying label at index " << i << std::endl;
        std::cout << "label: " << listOfContexts[i].c_str() << std::endl;
        chartextBoxes[i]->copy_label(listOfContexts[i].c_str());
    }
    // if there's an empty box, fill it with N/A
    if (listSize < boxSize) {
        for (int i = listSize; i < boxSize; i++) {
            chartextBoxes[i]->copy_label("N/A");
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
std::string ChoicePage::getConText(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    if (indx < 0 || indx >= thisPageLength) {
        std::cout << "bad index with: " << indx << std::endl;
        return "N/A";
    }

    // get the pointers for this run and assign them
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;

    // get the relative position of the bad character
    int32_t leftDiff = indx - leftPointer;
    // bad character at position:
    icu::UnicodeString posNotif = "Pos: ";
    icu::UnicodeString strNumber = icu::UnicodeString::fromUTF8(std::to_string(leftDiff + 1));
    posNotif += strNumber;
    posNotif += " ";

    // get the context and make a string to underline it
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);
    icu::UnicodeString invisiString(context.length(), '-');

    // add arrow pointing to the bad character
    if (leftDiff >= 0 && leftDiff < context.length()) {
        invisiString.setCharAt(leftDiff, '^');
    } else {
        std::cerr << "bad char is out of bounds" << std::endl;
    }

    // combine, convert to std::string, and return
    icu::UnicodeString combined = posNotif + "\n" + context + "\n" + invisiString;
    std::string combinedStr;
    combined.toUTF8String(combinedStr);

    return combinedStr;
}
std::vector<std::string> ChoicePage::getBintexts() {
    // if bindex is out of bounds, return an empty vector
    if (bindex >= uBadChars.length()) {
        std::cout << "bindex out of bounds at getBintexts()" << std::endl;
        return {};
    } else{
        // otherwise, get the current bad character
        UChar32 realBadChar = uBadChars[bindex];
        // get whichever is number is smaller - number of char occurences, or 3
        int numExamples = std::min(uCharOccurs[realBadChar], 3);

        // debugging prints
        std::cout << "numExamples: " << numExamples << std::endl;
        std::cout << "realBadChar: " << realBadChar << std::endl;
        std::cout << "pdfText size: " << uPdfText.length() << std::endl;

        // vars to store the indices of the bad characters
        std::vector<int32_t> indices; // where we'll store character indices

        // try to .find() the bad character in the pdfText - starting from char 0
        int32_t minDex = 0;
        for(int i = 0; i < numExamples; i++) {
            // try to find the next occurrence of this character
            int32_t thisDex = uPdfText.indexOf(realBadChar, minDex);

            // if we found it, add it to the list of indices
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::string thisContext = getConText(indices[i], uPdfText);
            contextList.push_back(thisContext);
        }

        // return a vector containing the index of the bad character
        return contextList;
    }
}
