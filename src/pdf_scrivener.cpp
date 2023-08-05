#include <iostream>
#include <fstream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

/*
- i initially added this because I thought we needed it for ligatures
- turns out you just need to convert to utf-8 when extracting from the page
- but we're gonna keep this here just in case things get *really* funky
#include <unicode/unistr.h>  // For icu::UnicodeString
#include <unicode/ustream.h> // For streaming operators
*/

int main() {
    // the pdf we use for testing
    std::string pdfInputs = "bin/inputs/";
    std::string pdfFileName = "GoodHeart";
    std::string pdfFilePath = pdfInputs + pdfFileName + ".pdf";

    // load the pdf with poppler
    std::unique_ptr<poppler::document> doc(poppler::document::load_from_file(pdfFilePath));

    // check if the pdf was loaded correctly
    if (!doc.get()) {
        std::cerr << "ERROR: Could not load file " << pdfFilePath << std::endl;
        return 1;
    }

    // sample pages to check if ligature is working
    for (int i = 170; i < 180; ++i) {

        // load current page, then open a .txt file for output
        std::unique_ptr<poppler::page> pg(doc->create_page(i));
        std::ofstream outer("bin/outputs/" + pdfFileName + "page" + std::to_string(i) + ".txt");

        // if page is loaded correctly
        if (pg.get()) {
            // extract page text byes in utf8 encoding
            poppler::byte_array byteArray = pg->text().to_utf8();

            // capture the byte array in a string std::string
            std::string popplerByteString(byteArray.begin(), byteArray.end());

            /*
            // VESTIGIAL - we do not really need this right now. keeping just in case
            // convert the UTF-8 text to an ICU Unicode (UTF-16) string
            icu::UnicodeString uText = icu::UnicodeString::fromUTF8(popplerByteString);

            // convert from UTF-16 to UTF-8
            std::string utf8Text;
            uText.toUTF8String(utf8Text);
            */

            // write the text to our .txt file
            outer << popplerByteString;
        } else {
            // if page isn't loading then print to terminal
            std::cerr << "ERROR: Could not load page " << i << std::endl;
        }
        // close the .txt file
        outer.close();
    }


    // put this code out of its misery
    return 0;
}

/*
#include "pdf_scrivener.h"

// create an empty vector for strings
std::vector<std::string> extractedText;

// good characters
std::string printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c";
std::string bad_chars; // we'll fill this in with non-printable characters
std::string ligatures = "ae, oe, fi, fl, and ffl";

std::string pdfFilePath;
// bad characters
Fl_Output *output;

#include <fstream>

void OpenPDF(Fl_Widget*, void*) {
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Open PDF file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("PDF\t*.pdf");
    switch ( fnfc.show() ) {
        case -1: break; // ERROR
        case  1: break; // CANCEL
        default:        // PICKED FILE
            pdfFilePath = fnfc.filename();

            // extract text from pdf
            TextExtraction pdfData;
            if (pdfData.ExtractText(pdfFilePath) != PDFHummus::eSuccess) {
                output->value("Failed to load PDF file");
                return;
            }

            // Get the text of page 5
            ParsedTextPlacementListList pdfPages = pdfData.textsForPages;
            if (pdfPages.size() < 5) {
                output->value("PDF file has less than 5 pages");
                return;
            }
            auto page2 = pdfPages.begin();
            std::advance(page2, 2);
            std::string page2Text;

            // Open file to write bad characters
            std::ofstream badCharFile("bad_characters.txt");

            for (const auto& placement : *page2) {
                // if character not in printable, add to bad_chars
                if (printable.find(placement.text) == std::string::npos) {
                    bad_chars += placement.text;
                    badCharFile << placement.text; // Write to file
                    std::cout << '?';
                    page2Text += '?';

                } else{
                    std::cout << placement.text;
                    page2Text += placement.text;
                }
            }

            // Close the bad characters file
            badCharFile.close();

            std::cout << "All done" << std::endl;
            // Display the text of page 5
            output->value(page2Text.c_str());
            break;
    }
}


int main() {
    Fl_Window *window = new Fl_Window(340,180);
    Fl_Button *button = new Fl_Button(10, 10, 80, 25, "Open PDF");
    button->callback(OpenPDF);
    output = new Fl_Output(10, 45, 320, 130);
    window->end();
    window->show();
    return Fl::run();
}

*/