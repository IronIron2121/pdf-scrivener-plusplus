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