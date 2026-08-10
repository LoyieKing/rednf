// Restored header for zen_os_adapt_string.cpp (gunnersvr oracle).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_

#include <stddef.h>
#include <time.h>

namespace ZEN_OS {
    extern void tm_from_str1(const char *strtm, tm *ptmptm);
    extern void tm_from_str2(const char *strtm, tm *ptmptm);
    extern void tm_from_str3(const char *strtm, tm *ptmptm);
    extern void tm_from_str4(const char *strtm, tm *ptmptm);
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
    extern time_t time_from_str4(const char *strtm);
    extern time_t time_from_str3(const char *strtm);
    extern time_t time_from_str2(const char *strtm);
    extern time_t time_from_str1(const char *strtm);
} // namespace ZEN_OS

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_OS_ADAPT_STRING_H_H_
