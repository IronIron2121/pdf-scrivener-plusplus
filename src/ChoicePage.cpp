#include "ChoicePage.h"
#include "AppWizard.h"
#include "ContextPage.h"
#include <iostream>


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
    uCharOccursHere = parent->getUCharOccurs();
    newPdfTextHere = parent->getNewPdfText();
    newPdfListHere = parent->getNewPdfList();
    uPrintableHere = parent->getUPrintable();
    uNewLinesHere = parent->getUNewLines();

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

void ChoicePage::goodifyCb() { 
    // i apologise sincerely for this. there was no other way

    // non-contextual replacement
    ((*replacementDictHere)[(*uBadCharsHere)[*bindexHere]]).contextual = false;

    // replacement is the character itself...converted from uChar to uString
    icu::UnicodeString thisRep = icu::UnicodeString(getCurrChar());

    ((*replacementDictHere)[(*uBadCharsHere)[*bindexHere]]).replacement = thisRep; 

    icu::UnicodeString postRep = ((*replacementDictHere)[(*uBadCharsHere)[*bindexHere]]).replacement;
    std::string postRepStr;
    postRep.toUTF8String(postRepStr);
    std::cout << "replaced with IN DICT: " << postRepStr << std::endl;

    nextChar();
}


void ChoicePage::replaceAllCb() {
    // non-contextual replacement
    ((*replacementDictHere)[(*uBadCharsHere)[*bindexHere]]).contextual = false;
    // replace every instance of this character with the user's input
    // convert input to UChar32
    std::string inputStr = this->replaceAllInput->value();
    icu::UnicodeString inputU(inputStr.c_str());

    ((*replacementDictHere)[(*uBadCharsHere)[*bindexHere]]).replacement = inputU;
    nextChar();

}

void ChoicePage::contextCb() {
    ContextPage* thisContextPage = new ContextPage(0, 0, 1000, 800, parent, "Contextual Replacement");
    thisContextPage->newInit();  // Refresh before showing.
    thisContextPage->show();
}


// get the current character
UChar32 ChoicePage::getCurrChar() {
    UChar32 badChar = (*uBadCharsHere)[*bindexHere];
    return badChar;
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


void ChoicePage::nextChar() {
    // increment the bad character index
    (*bindexHere)++;
    // std::cout << "bindexHere Next Char: " << *bindexHere << std::endl;

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


void ChoicePage::refreshVals() {
    // update bad char display
    std::string newText = "Current Character: " + std::string(getDisplayChar());
    std::cout << "new text: " << newText << std::endl;

    Fl_Box *thisCharLabel = getCharLabel();
    thisCharLabel->copy_label(newText.c_str());


    // grab the character context boxes
    std::vector<Fl_Box*> chartextBoxes = getChartextBoxes();

    // get the contexts for the current bad character
    std::vector<std::string> listOfContexts = getBintexts();

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
std::tuple<std::string, int, int, icu::UnicodeString> ChoicePage::getConText(int indx, const icu::UnicodeString& pageText) {
    int32_t thisPageLength = pageText.length();

    // error handling
    if (indx < 0 || indx >= thisPageLength) {
        std::cerr << "bad index with: " << indx << std::endl;
        return std::make_tuple("", -1, -1, "");
    }

    // get the pointers for this run and assign them
    std::pair<int32_t,int32_t> pointers = getPointers(indx, pageText, thisPageLength);
    int32_t leftPointer = pointers.first;
    int32_t rightPointer = pointers.second;

    // get the relative position of the bad character
    int32_t leftDiff = indx - leftPointer;

    // construct a string that tells users which char in a string they're meant to be looking at
    icu::UnicodeString posNotif = "Pos: ";
    icu::UnicodeString strNumber = icu::UnicodeString::fromUTF8(std::to_string(leftDiff + 1));
    posNotif += strNumber;
    posNotif += " ";

    // extract the context 
    icu::UnicodeString context = pageText.tempSubString(leftPointer, rightPointer - leftPointer);
    // get the size of the context
    int32_t contextSize = context.length();

    // combine positional information with context phrase, then convert to std::string
    icu::UnicodeString combined = posNotif + "\n" + context;
    std::string combinedStr;
    combined.toUTF8String(combinedStr);

    // pack together the context and the relative position of the bad character
    std::tuple<std::string, int, int, icu::UnicodeString> thisOut = std::make_tuple(combinedStr, leftDiff, contextSize, context);

    return thisOut;
}
std::vector<std::string> ChoicePage::getBintexts() {
    // if bindex is out of bounds, return an empty vector
    if (*bindexHere >= (*uBadCharsHere).length()) {
        std::cout << "bindex out of bounds at getBintexts()" << std::endl;
        return {};
    } else{
        // otherwise, get the current bad character
        UChar32 realBadChar = getCurrChar();

        // get whichever is number is smaller - number of char occurences, or 3
        int numExamples = std::min((*uCharOccursHere)[realBadChar], 3);

        // var to store the indices of the bad characters
        std::vector<int32_t> indices;

        // try to .find() the bad character in the pdfText - starting from char 0
        int32_t minDex = 0;

        // for every example we wanna make
        for(int i = 0; i < numExamples; i++) {
            // try to find the next occurrence of this character
            int32_t thisDex = (*newPdfTextHere).indexOf(realBadChar, minDex);

            // if we found it, add it to the list of indices
            if (thisDex != std::string::npos) {
                indices.push_back(thisDex);
                minDex = thisDex + 1;
            }
        }

        // get the context of each bad character
        std::vector<std::string> contextList;
        for(int i = 0; i < indices.size(); i++) {
            std::tuple<std::string, int, int, icu::UnicodeString> contOut= getConText(indices[i], (*newPdfTextHere));
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

                    //icu::UnicodeString replacement = (*contextDictHere)[thisChar][context];
                    // modText += replacement;
                    // charIndex += U16_LENGTH(thisChar);  // you know the drill

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