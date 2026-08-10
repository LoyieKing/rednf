// Restored from secagent binary (DWARF + disassembly), GCC 4.1.0 SUSE, -O2.
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_os_adapt_string.cpp
// Original quirks preserved (verified against the oracle at 0x08169df0-0x0816a4e9):
//  - tm_from_str/time_from_str dispatch on TIME_STRING_FORMAT (TSF_*).
//  - tm_from_str zeroes *usec first, and only computes it for TSF_YYYY_MM_DD_HH_MM_SS_UUUUUU.
//  - strlwr converts chars starting at index 1; str[0] is never changed.
//  - skip_separator is byte-identical to the removed 2-arg skip_token.

#include "src/commlib/zenlib/zen_os_adapt_string.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

namespace ZEN_OS {

// getpid is a thin wrapper defined in zen_os_adapt_process.cpp.
extern int getpid();

// File-static counter used by prefix_unique_name (BSS in the oracle).
static unsigned int uniqueid_builder = 0;

void tm_from_str(TIME_STRING_FORMAT fmt, const char *strtm, tm *ptr_tm,
                 time_t *usec) {
    if (usec != NULL) {
        *usec = 0;
    }
    if (fmt <= TSF_YYYYMMDDHHMMSS) {
        // Compact digits, no separators: YYYYMMDD[HHMMSS].
        ptr_tm->tm_isdst = 0;
        ptr_tm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                          (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
        ptr_tm->tm_mon = (strtm[4] - '0') * 10 + (strtm[5] - '0') - 1;
        ptr_tm->tm_mday = (strtm[6] - '0') * 10 + (strtm[7] - '0');
        if (fmt == TSF_YYYYMMDD) {
            ptr_tm->tm_hour = 0;
            ptr_tm->tm_min = 0;
            ptr_tm->tm_sec = 0;
        } else {
            ptr_tm->tm_hour = (strtm[8] - '0') * 10 + (strtm[9] - '0');
            ptr_tm->tm_min = (strtm[10] - '0') * 10 + (strtm[11] - '0');
            ptr_tm->tm_sec = (strtm[12] - '0') * 10 + (strtm[13] - '0');
        }
        return;
    }
    if (fmt <= TSF_YYYY_MM_DD_HH_MM_SS_UUUUUU) {
        // ISO style, separators skipped at fixed positions:
        // YYYY-MM-DD[ HH:MM:SS[.UUUUUU]].
        ptr_tm->tm_isdst = 0;
        ptr_tm->tm_year = (strtm[0] - '0') * 1000 + (strtm[1] - '0') * 100 +
                          (strtm[2] - '0') * 10 + (strtm[3] - '0') - 1900;
        ptr_tm->tm_mon = (strtm[5] - '0') * 10 + (strtm[6] - '0') - 1;
        ptr_tm->tm_mday = (strtm[8] - '0') * 10 + (strtm[9] - '0');
        if (fmt == TSF_YYYY_MM_DD) {
            ptr_tm->tm_hour = 0;
            ptr_tm->tm_min = 0;
            ptr_tm->tm_sec = 0;
        } else {
            ptr_tm->tm_hour = (strtm[11] - '0') * 10 + (strtm[12] - '0');
            ptr_tm->tm_min = (strtm[14] - '0') * 10 + (strtm[15] - '0');
            ptr_tm->tm_sec = (strtm[17] - '0') * 10 + (strtm[18] - '0');
        }
    }
    if (usec != NULL && fmt == TSF_YYYY_MM_DD_HH_MM_SS_UUUUUU) {
        // Microseconds from strtm[20..25] (fraction after the '.' at [19]).
        *usec = (strtm[20] - '0') * 100000 + (strtm[21] - '0') * 10000 +
                (strtm[22] - '0') * 1000 + (strtm[23] - '0') * 100 +
                (strtm[24] - '0') * 10 + (strtm[25] - '0');
    }
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

const char *skip_separator(const char *str, char separator_char) {
    // Oracle quirk: identical to the removed 2-arg skip_token (stops after
    // one separator).
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
    return ::strcasecmp(string1, string2);
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

time_t time_from_str(TIME_STRING_FORMAT fmt, const char *strtm) {
    tm tm_value;
    tm_from_str(fmt, strtm, &tm_value, NULL);
    return mktime(&tm_value);
}

} // namespace ZEN_OS
