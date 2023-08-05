#include "pdf_scrivener.h"

// create an empty vector for strings
std::vector<std::string> extractedText;

// good characters
std::string printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c";
std::string bad_chars; // we'll fill this in with non-printable characters

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