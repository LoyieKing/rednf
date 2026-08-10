// Restored from gunnersvr binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_os_adapt_string.cpp
// Original quirks preserved (verified against the oracle at 0x08091ef0-0x080928d8):
//  - strcasecmp passes string1 for BOTH arguments (always returns 0).
//  - strlwr converts chars starting at index 1; str[0] is never changed.
//  - skip_separator is byte-identical to skip_token (both stop at one separator).
//  - tm_from_str3/4 skip the '-'/' ' ':' separators at fixed positions.

#include "src/commlib/zenlib/zen_os_adapt_string.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

namespace ZEN_OS {

// getpid is a thin wrapper defined in zen_os_adapt_process.cpp.
extern int getpid();

// File-static counter used by prefix_unique_name (BSS in the oracle).
static unsigned int uniqueid_builder = 0;

void tm_from_str1(const char *strtm, tm *ptmptm) {
    ptmptm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                      (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
    ptmptm->tm_mon = (strtm[4] - '0') * 10 + (strtm[5] - '0') - 1;
    ptmptm->tm_mday = (strtm[6] - '0') * 10 + (strtm[7] - '0');
    ptmptm->tm_hour = 0;
    ptmptm->tm_min = 0;
    ptmptm->tm_sec = 0;
    ptmptm->tm_isdst = 0;
}

void tm_from_str2(const char *strtm, tm *ptmptm) {
    ptmptm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                      (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
    ptmptm->tm_mon = (strtm[4] - '0') * 10 + (strtm[5] - '0') - 1;
    ptmptm->tm_mday = (strtm[6] - '0') * 10 + (strtm[7] - '0');
    ptmptm->tm_hour = (strtm[8] - '0') * 10 + (strtm[9] - '0');
    ptmptm->tm_min = (strtm[10] - '0') * 10 + (strtm[11] - '0');
    ptmptm->tm_sec = (strtm[12] - '0') * 10 + (strtm[13] - '0');
    ptmptm->tm_isdst = 0;
}

void tm_from_str3(const char *strtm, tm *ptmptm) {
    // Format: YYYY-MM-DD ('-' at strtm[4] and strtm[7] is skipped).
    ptmptm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                      (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
    ptmptm->tm_mon = (strtm[5] - '0') * 10 + (strtm[6] - '0') - 1;
    ptmptm->tm_mday = (strtm[8] - '0') * 10 + (strtm[9] - '0');
    ptmptm->tm_hour = 0;
    ptmptm->tm_min = 0;
    ptmptm->tm_sec = 0;
    ptmptm->tm_isdst = 0;
}

void tm_from_str4(const char *strtm, tm *ptmptm) {
    // Format: YYYY-MM-DD HH:MM:SS (separators at strtm[4], [7], [10], [13], [16]).
    ptmptm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                      (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
    ptmptm->tm_mon = (strtm[5] - '0') * 10 + (strtm[6] - '0') - 1;
    ptmptm->tm_mday = (strtm[8] - '0') * 10 + (strtm[9] - '0');
    ptmptm->tm_hour = (strtm[11] - '0') * 10 + (strtm[12] - '0');
    ptmptm->tm_min = (strtm[14] - '0') * 10 + (strtm[15] - '0');
    ptmptm->tm_sec = (strtm[17] - '0') * 10 + (strtm[18] - '0');
    ptmptm->tm_isdst = 0;
}

const char *skip_line(const char *str) {
    while (*str && *str != '\n') {
        ++str;
    }
    if (*str == '\n') {
        ++str;
    }
    return str;
}

const char *skip_token(const char *str, char separator_char) {
    while (*str && *str != separator_char) {
        ++str;
    }
    if (*str == separator_char) {
        ++str;
    }
    return str;
}

const char *skip_separator(const char *str, char separator_char) {
    // Oracle quirk: identical to skip_token (stops after one separator).
    while (*str && *str != separator_char) {
        ++str;
    }
    if (*str == separator_char) {
        ++str;
    }
    return str;
}

const char *skip_token(const char *str) {
    while (isspace((unsigned char)*str)) {
        ++str;
    }
    if (*str == '\0') {
        return str;
    }
    while (*str && !isspace((unsigned char)*str)) {
        ++str;
    }
    return str;
}

const char *skip_whitespace(const char *str) {
    while (isspace((unsigned char)*str)) {
        ++str;
    }
    return str;
}

char *strtrimright(char *str) {
    char *lstr = str + strlen(str) - 1;
    char *str_end = lstr;
    if (str > lstr) {
        return str;
    }
    while (isspace((unsigned char)*lstr)) {
        --lstr;
        if (lstr < str) {
            break;
        }
    }
    if (lstr != str_end) {
        lstr[1] = '\0';
    }
    return str;
}

char *strtrimleft(char *str) {
    char *lstr = str;
    while (*lstr && isspace((unsigned char)*lstr)) {
        ++lstr;
    }
    if (lstr == str) {
        return str;
    }
    memmove(str, lstr, strlen(lstr) + 1);
    return str;
}

char *strtrim(char *str) {
    strtrimleft(str);
    strtrimright(str);
    return str;
}

int strncasecmp(const char *string1, const char *string2, size_t maxlen) {
    return ::strncasecmp(string1, string2, maxlen);
}

int strcasecmp(const char *string1, const char *string2) {
    // Oracle quirk: both arguments are string1 (result is always 0).
    return ::strcasecmp(string1, string1);
}

char *strlwr(char *str) {
    char *lstr = str;
    if (*str == '\0') {
        return str;
    }
    // Oracle quirk: converts str[1] onward; str[0] is left unchanged.
    do {
        lstr[1] = tolower(lstr[1]);
        ++lstr;
    } while (*lstr != '\0');
    return str;
}

char *strupr(char *str) {
    char *lstr = str;
    while (*lstr) {
        *lstr = toupper(*lstr);
        ++lstr;
    }
    return str;
}

char *prefix_unique_name(const char *prefix_name, char *name, size_t length) {
    ++uniqueid_builder;
    snprintf(name, length, "%s.%u.%x", prefix_name, ZEN_OS::getpid(),
             uniqueid_builder);
    return name;
}

char *object_unique_name(const void *object_ptr, char *name, size_t length) {
    snprintf(name, length, "%u.%p", ZEN_OS::getpid(), object_ptr);
    return name;
}

time_t time_from_str4(const char *strtm) {
    int year = -1;
    int mon = -1;
    int day = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;
    sscanf(strtm, "%4d-%2d-%2d %2d:%2d:%2d",
           &year, &mon, &day, &hour, &min, &sec);
    tm tmtmp;
    tmtmp.tm_year = year - 1900;
    tmtmp.tm_mon = mon - 1;
    tmtmp.tm_mday = day;
    tmtmp.tm_hour = hour;
    tmtmp.tm_min = min;
    tmtmp.tm_sec = sec;
    tmtmp.tm_isdst = 0;
    return mktime(&tmtmp);
}

time_t time_from_str3(const char *strtm) {
    int year = -1;
    int mon = -1;
    int day = -1;
    sscanf(strtm, "%4d-%2d-%2d", &year, &mon, &day);
    tm tmtmp;
    tmtmp.tm_hour = 0;
    tmtmp.tm_min = 0;
    tmtmp.tm_sec = 0;
    tmtmp.tm_mday = day;
    tmtmp.tm_mon = mon - 1;
    tmtmp.tm_year = year - 1900;
    tmtmp.tm_isdst = 0;
    return mktime(&tmtmp);
}

time_t time_from_str2(const char *strtm) {
    int year = -1;
    int mon = -1;
    int day = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;
    sscanf(strtm, "%4d%2d%2d%2d%2d%2d",
           &year, &mon, &day, &hour, &min, &sec);
    tm tmtmp;
    tmtmp.tm_year = year - 1900;
    tmtmp.tm_mon = mon - 1;
    tmtmp.tm_mday = day;
    tmtmp.tm_hour = hour;
    tmtmp.tm_min = min;
    tmtmp.tm_sec = sec;
    tmtmp.tm_isdst = 0;
    return mktime(&tmtmp);
}

time_t time_from_str1(const char *strtm) {
    int year = -1;
    int mon = -1;
    int day = -1;
    sscanf(strtm, "%4d%2d%2d", &year, &mon, &day);
    tm tmtmp;
    tmtmp.tm_hour = 0;
    tmtmp.tm_min = 0;
    tmtmp.tm_sec = 0;
    tmtmp.tm_mday = day;
    tmtmp.tm_mon = mon - 1;
    tmtmp.tm_year = year - 1900;
    tmtmp.tm_isdst = 0;
    return mktime(&tmtmp);
}

} // namespace ZEN_OS
