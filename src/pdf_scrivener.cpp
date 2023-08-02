#include <fstream>
#include <wx/wxprec.h>

// file dialog, filestream
#include <wx/filedlg.h> 
#include <wx/wfstream.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include "TextExtraction.h"
#include <regex>
#include <iostream>

// create an empty vector for strings
std::vector<std::string> extractedText;
std::string printable = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ \t\n\r\x0b\x0c";
std::string pdfFilePath = "./bin/inputs/GoodHeart.pdf";
std::string bad_chars;


#include <wx/wx.h>
#include <wx/filedlg.h>

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnOpen(wxCommandEvent& event);

private:
    wxButton* m_button;
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_BUTTON(wxID_OPEN, MyFrame::OnOpen)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "PDF Scrivener", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    m_button = new wxButton(this, wxID_OPEN, "Open PDF", wxPoint(10, 10));
}

void MyFrame::OnOpen(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(this, _("Open PDF file"), "", "", "PDF files (*.pdf)|*.pdf", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // if user cancels

    // get selected pdf
    wxString wxFilePath = openFileDialog.GetPath();
    std::string pdfFilePath(wxFilePath.mb_str());

    // extract text from pdf
    TextExtraction pdfData;
    if (pdfData.ExtractText(pdfFilePath) != PDFHummus::eSuccess) {
        wxLogMessage("Failed to load PDF file");
        return;
    }

    // Get the text of page 5
    ParsedTextPlacementListList pdfPages = pdfData.textsForPages;
    if (pdfPages.size() < 5) {
        wxLogMessage("PDF file has less than 5 pages");
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
    wxMessageBox(page5Text, "Page 5 Text", wxOK | wxICON_INFORMATION);
}


/*
int main() {

    std::cout << "Starting main()" << std::endl;

    // extract text from provided pdf file
    TextExtraction pdfData;
    if (pdfData.ExtractText(pdfFilePath) != PDFHummus::eSuccess) {
        std::cout << "PDF FAILED TO LOAD" << std::endl;
    } else {
        std::cout << "PDF LOADED SUCCESSFULLY" << std::endl;
    }

    // extract text data from pdfData object
    ParsedTextPlacementListList pdfPages = pdfData.textsForPages;

    // make an iterator for the ParsedTextPlacementListList
    ParsedTextPlacementListList::iterator itPage = pdfPages.begin();

    // for loop that iterates over the pages of the pdf, and extracts the text from each page, and appends it to a vector
    for(; itPage != pdfPages.end(); ++itPage) {
        // for each page, create an empty string
        std::string thisPage;

        // create an iterator for the characters on the page
        ParsedTextPlacementList::iterator itChar = itPage->begin();

        for(; itChar != itPage->end(); ++itChar) {
            // for each ParsedTextPlacement object, get the text
            std::vector<char> thisChar(itChar->text.begin(), itChar->text.end());
            
            // append the extracted text to thisPage
            for (char& c : thisChar) {
                if (printable.find(c) == std::string::npos) {
                    std::cout << "INVALID CHARACTER" << std::endl;
                    bad_chars += c;
                    thisPage += "WHAT???";
                    } else{
                    thisPage += c;
                    }
            }
        }
        // append the page to our vector
        extractedText.push_back(thisPage);
    }

    int pageNumber = 5;

    std::ofstream outFile("output.txt");

    if(outFile.is_open()) {
        outFile << extractedText[pageNumber];
        outFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    std::cout << "Finished main()" << std::endl;


    return 0;
}

*/