#include "pdf_scrivener.h"
#include "AppWizard.h"

// -------------------------------------------------------------------------- //
int main(int argc, char** argv) {
    AppWizard appWin(1000, 800, "PDF Scrivener");
    appWin.show(argc, argv);
    return Fl::run();
}
// -------------------------------------------------------------------------- //
