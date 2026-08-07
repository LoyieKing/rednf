#ifndef REDNF_SECURESTDIO_H_
#define REDNF_SECURESTDIO_H_

#include <stdarg.h>
#include <stddef.h>

int ss_sprintf(char* buffer, size_t count, const char* format, ...);
int ss_swprintf(wchar_t* buffer, size_t count, const wchar_t* format, ...);
int ss_vsprintf(char* buffer, size_t count, const char* format, char* ap);
int ss_vswprintf(wchar_t* buffer, size_t count, const wchar_t* format, char* ap);
int ss_strcat(char* dest, size_t count, const char* src);
int ss_strcpy(char* dest, size_t count, const char* src);
int ss_wcscat(wchar_t* dest, size_t count, const wchar_t* src);
int ss_wcscpy(wchar_t* dest, size_t count, const wchar_t* src);

#endif // REDNF_SECURESTDIO_H_
