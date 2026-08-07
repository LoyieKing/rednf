#ifndef UNICODECONVERT_H_
#define UNICODECONVERT_H_

#include <string>
#include "Strings.h"

int getCodePage();
int setCodePage(int nNewCodePage);
char* getMbcsBuffer();
wchar_t* getUnicodeBuffer();
char* getTCharBuffer();
int getMbcsBufferSize();
int getUnicodeBufferSize();
int getTCharBufferSize();

const char* toTString(const std::string& sSrc);
const char* toTString(const std::wstring& sSrc);
const char* toTString(const char* pszSrc);
const char* toTString(const wchar_t* pszSrc);
void toTString(const std::string& sSrc, std::string& sDst);
void toTString(const std::wstring& sSrc, std::string& sDst);
void toTString(const char* pszSrc, std::string& sDst);
void toTString(const wchar_t* pszSrc, std::string& sDst);

const char* toMbcs(const std::string& sSrc);
const char* toMbcs(const std::wstring& sSrc);
const char* toMbcs(const char* pszSrc);
const char* toMbcs(const wchar_t* pszSrc);
void toMbcs(const std::string& sSrc, std::string& sDst);
void toMbcs(const std::wstring& sSrc, std::string& sDst);
void toMbcs(const char* pszSrc, std::string& sDst);
void toMbcs(const wchar_t* pszSrc, std::string& sDst);

const wchar_t* toUnicode(const std::string& sSrc);
const wchar_t* toUnicode(const std::wstring& sSrc);
const wchar_t* toUnicode(const char* pszSrc);
const wchar_t* toUnicode(const wchar_t* pszSrc);
void toUnicode(const std::string& sSrc, std::wstring& sDst);
void toUnicode(const std::wstring& sSrc, std::wstring& sDst);
void toUnicode(const char* pszSrc, std::wstring& sDst);
void toUnicode(const wchar_t* pszSrc, std::wstring& sDst);

WideString convertToWideString(const char* src);
WideString convertToWideString(const char* src, uint32 codePage);
WideString convertToWideString(const CharString& src);
WideString convertToWideString(const CharString& src, uint32 codePage);
WideString convertToWideString(const WideString& src);

CharString convertToCharString(const wchar* src);
CharString convertToCharString(const wchar* src, uint32 codePage);
CharString convertToCharString(const WideString& src);
CharString convertToCharString(const WideString& src, uint32 codePage);
CharString convertToCharString(const CharString& src);

bool convertToUtf8(const wchar* source, int32 sourceLen, CharString* dest);
bool convertFromUtf8(const char* source, int32 sourceLen, WideString* dest);

#endif // UNICODECONVERT_H_
