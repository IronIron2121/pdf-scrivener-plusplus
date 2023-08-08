#ifndef REPLACEMENTINFO_H
#define REPLACEMENTINFO_H

// file for the replacement dict struct
#include <unicode/uchar.h>
#include <string>

struct ReplacementInfo {
    bool contextual; // is this replacement context sensitive?
    std::string replacement; // what is the replacement?
};

#endif