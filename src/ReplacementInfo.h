#ifndef REPLACEMENTINFO_H
#define REPLACEMENTINFO_H

// file for the replacement dict struct
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>
#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/utf8.h>
#include <unicode/utf16.h>

#include <string>

struct ReplacementInfo {
    bool contextual; // is this replacement context sensitive?
    icu::UnicodeString replacement; // what is the replacement?
};

#endif