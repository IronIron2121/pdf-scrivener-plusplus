#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Output.H>
#include "TextExtraction.h"
#include <string>
#include <vector>

// create an empty vector for strings
std::vector<std::string> extractedText;
std::string printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c";
std::string pdfFilePath = "./bin/inputs/GoodHeart.pdf";
std::string bad_chars;

Fl_Output *output;

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
            auto page5 = pdfPages.begin();
            std::advance(page5, 4);
            std::string page5Text;
            for(; page5 != pdfPages.end(); ++page5) {
                for (const auto& placement : *page5) {
                    page5Text += placement.text;
                }
            }
            // Display the text of page 5
            output->value(page5Text.c_str());
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
