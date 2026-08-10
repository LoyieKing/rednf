// Restored from secagent binary (DWARF + disassembly), GCC 4.1.0 SUSE.
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_

#include <stddef.h>
#include <time.h>

namespace ZEN_OS {
    enum TIME_STRING_FORMAT {
        TSF_YYYYMMDD = 0,
        TSF_YYYYMMDDHHMMSS = 1,
        TSF_YYYY_MM_DD = 2,
        TSF_YYYY_MM_DD_HH_MM_SS = 3,
        TSF_YYYY_MM_DD_HH_MM_SS_UUUUUU = 4,
    };

    extern void tm_from_str(TIME_STRING_FORMAT fmt, const char *strtm, tm *ptr_tm, time_t *usec);
    extern time_t time_from_str(TIME_STRING_FORMAT fmt, const char *strtm);
    extern const char *skip_line(const char *str);
    extern const char *skip_token(const char *str, char separator_char);
    extern const char *skip_separator(const char *str, char separator_char);
    extern const char *skip_token(const char *str);
    extern const char *skip_whitespace(const char *str);
    extern char *strtrimright(char *str);
    extern char *strtrimleft(char *str);
    extern char *strtrim(char *str);
    extern int strncasecmp(const char *string1, const char *string2, size_t maxlen);
    extern int strcasecmp(const char *string1, const char *string2);
    extern char *strlwr(char *str);
    extern char *strupr(char *str);
    extern char *prefix_unique_name(const char *prefix_name, char *name, size_t length);
    extern char *object_unique_name(const void *object_ptr, char *name, size_t length);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_
