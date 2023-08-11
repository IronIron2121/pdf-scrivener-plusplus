#include "pdf_scrivener.h"
#include "AppWizard.h"

int main(int argc, char** argv) {
    int x, y, w, h;
    int screenNum = 0; // open up on main monitor

    // launch on first screen
    Fl::screen_xywh(x, y, w, h, screenNum);
    AppWizard appWin(1000, 800, "PDF Scrivener");
    appWin.position(x, y);
    appWin.show(argc, argv);
    return Fl::run();
}
