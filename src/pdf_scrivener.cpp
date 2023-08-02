// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "TextExtraction.h"
#include <regex>
#include <iostream>

std::string printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c";
std::string pdfFilePath = "./bin/inputs/GoodHeart.pdf";



int main() {
    TextExtraction textExtractor;

    // Extract text placements from the PDF
    if (textExtractor.ExtractTextPlacements(pdfFilePath, 0, -1) != PDFHummus::eSuccess) {
        std::cerr << "Failed to extract text placements from the PDF file." << std::endl;
        return 1;
    }

    // Get the extracted text for each page and print it
    std::list<ParsedTextPlacementList> textsForPages = textExtractor.GetTextsForPages();
    int pageNum = 1;
    for (const auto& pageText : textsForPages) {
        std::cout << "Page " << pageNum << ":\n";
        for (const auto& textPlacement : pageText) {
            std::cout << textPlacement.text << "\n";
        }
        std::cout << "\n\n";
        pageNum++;
    }

    return 0;
}

/*
std::string clean_text(const std::string& pdfFilePath) {
    TextExtraction textExtraction;
    if (textExtraction.ExtractText(pdfFilePath) != PDFHummus::eSuccess) {
        std::cout << "PDF FAILED TO LOAD" << std::endl;
    }

    std::string extractedText = textExtraction.GetResultsAsText(0, TextComposer::eSpacingNone);

    // Define the bad characters
    std::string bad_chars = "\\/:*?\"<>|";

    // Replace bad characters with underscore
    for (char& c : extractedText) {
        if (bad_chars.find(c) != std::string::npos) {
            c = '_';
        }
    }

    // Replace multiple underscores with a single underscore
    std::regex multiple_underscores("_+");
    extractedText = std::regex_replace(extractedText, multiple_underscores, "_");

    return extractedText;
}

int main() {

    std::cout << "Starting main()" << std::endl;

    // put your pdf filepath here
    
    //  
    std::string cleanedText = clean_text(pdfFilePath);
    std::cout << cleanedText << std::endl;

    std::cout << "Text extraction complete" << std::endl;

    return 0;
}
*/