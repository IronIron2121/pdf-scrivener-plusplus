#ifndef MY_PAGE_H
#define MY_PAGE_H

#include <FL/Fl_Group.H>

class MyPage : public Fl_Group {
public:
    MyPage(int x, int y, int w, int h, const char* title = 0) : Fl_Group(x, y, w, h, title) {}
};

#endif