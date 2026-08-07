#ifndef STRINGS_H_
#define STRINGS_H_

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>

typedef int int32;
typedef unsigned int uint32;
typedef wchar_t wchar;

struct StringData
{
    volatile long refCount_;
    int32 size_;

    void incRef();
    void decRef();
    bool isUnique() const;
    bool isShared() const;
    int32 getSize() const;
    unsigned char* getBuffer();
    static StringData* create(int32 size);
};

struct CharStringData : public StringData
{
    int32 getLength() const;
    char* getBuffer();
    static CharStringData* create(int32 length);
    static CharStringData* createTerminated(int32 length);
};

struct WideStringData : public StringData
{
    int32 getLength() const;
    wchar* getBuffer();
    static WideStringData* create(int32 length);
    static WideStringData* createTerminated(int32 length);
};

class CharString
{
public:
    typedef const char* const_iterator;

    CharString();
    CharString(const CharString& src);
    CharString(const char* src);
    CharString(const char* src, int32 length);
    CharString(const std::string& src);
    CharString(char src);
    CharString(CharStringData* newData);
    ~CharString();

    const char* c_str() const
    {
        return buffer_;
    }
    int32 length() const
    {
        return getData()->getLength();
    }
    int32 size() const
    {
        return getData()->getLength();
    }
    bool empty() const
    {
        return length() == 0;
    }
    bool isEmpty() const
    {
        return length() == 0;
    }
    bool isNotEmpty() const
    {
        return length() != 0;
    }
    bool isWhiteSpace() const;
    uint32 getHash() const;
    char getAt(int32 idx) const;
    CharString setAt(int32 idx, char ch);
    bool startsWith(const char* pat, bool isIgnoreCase) const;
    bool endsWith(const char* pat, bool isIgnoreCase) const;
    char front() const;
    char back() const;
    const_iterator begin() const;
    const_iterator end() const;
    void assign(const CharString& src);
    void assign(const char* src);
    void assign(const char* src, int32 srcLength);
    void assign(char src);
    void clear();
    char operator[](int idx) const;
    operator const char*() const;
    CharString& operator=(const CharString& src);
    CharString& operator=(const char* src);
    CharString& operator=(const std::string& src);
    CharString& operator=(char src);
    void attachData(CharStringData* newData);
    void replaceData(CharStringData* newData);
    CharStringData* getData() const;
    const_iterator find(const char* src) const;
    const_iterator find(const char* src, int32 length) const;
    const_iterator find(int32 start, const char* src) const;
    const_iterator find(int32 start, const char* src, int32 length) const;
    const_iterator find(const CharString& src) const;
    const_iterator find(int32 start, const CharString& src) const;
    const_iterator find(char ch) const;
    const_iterator find(int32 start, char ch) const;
    const_iterator rfind(const char* src) const;
    const_iterator rfind(const char* src, int32 length) const;
    const_iterator rfind(int32 start, const char* src) const;
    const_iterator rfind(int32 start, const char* src, int32 length) const;
    const_iterator rfind(const CharString& src) const;
    const_iterator rfind(int32 start, const CharString& src) const;
    const_iterator rfind(char ch) const;
    const_iterator rfind(int32 start, char ch) const;
    static CharString concat(const char* src1, const char* src2);
    static CharString concat(const char* src1, int32 src1Len, const char* src2, int32 src2Len);
    static CharString concat(const CharString& src1, const char* src2);
    static CharString concat(const CharString& src1, const char* src2, int32 src2Len);
    static CharString concat(const char* src1, const CharString& src2);
    static CharString concat(const char* src1, int32 src1Len, const CharString& src2);
    static CharString concat(const CharString& src1, const CharString& src2);
    static CharString concat(const CharString& src1, char ch);
    static CharString concat(char ch, const CharString& src1);
    static CharString pattern(const char* pattern, int32 length);
    CharString format(const char* format, ...);
    CharString vformat(const char* format, char* argList);
    void tokenize(const CharString& delimiters, std::vector<CharString>* tokenVector) const;
    void tokenize(const CharString& delimiters, std::vector<CharString>* tokenVector,
                  const char* delimiters2, const char* delimiters3, bool bKeepEmpty) const;
    void tokenizeNewLine(const CharString& delimiters, std::vector<CharString>* tokenVector) const;
    CharString join(const std::vector<CharString>& stringVector, const CharString& separator) const;
    static int32 compare(const CharString& src1, const char* src2);
    static int32 compare(const CharString& src1, const CharString& src2);
    static int32 compare(const char* src1, const CharString& src2);
    static int32 compareNoCase(const CharString& src1, const char* src2);
    static int32 compareNoCase(const CharString& src1, const CharString& src2);
    static int32 compareNoCase(const char* src1, const CharString& src2);

    char* buffer_;
};

class WideString
{
public:
    typedef const wchar* const_iterator;

    WideString();
    WideString(const WideString& src);
    WideString(const wchar* src);
    WideString(const wchar* src, int32 length);
    WideString(const std::wstring& src);
    WideString(wchar src);
    WideString(WideStringData* newData);
    ~WideString();

    const wchar* c_str() const
    {
        return buffer_;
    }
    int32 length() const
    {
        return getData()->getLength();
    }
    int32 size() const
    {
        return getData()->getLength();
    }
    bool empty() const
    {
        return length() == 0;
    }
    bool isEmpty() const
    {
        return length() == 0;
    }
    bool isNotEmpty() const
    {
        return length() != 0;
    }
    bool isWhiteSpace() const;
    uint32 getHash() const;
    wchar getAt(int32 idx) const;
    WideString setAt(int32 idx, wchar ch);
    bool startsWith(const wchar* pat, bool isIgnoreCase) const;
    bool endsWith(const wchar* pat, bool isIgnoreCase) const;
    wchar front() const;
    wchar back() const;
    const_iterator begin() const;
    const_iterator end() const;
    void assign(const WideString& src);
    void assign(const wchar* src);
    void assign(const wchar* src, int32 srcLength);
    void assign(wchar src);
    void clear();
    wchar operator[](int idx) const;
    operator const wchar*() const;
    WideString& operator=(const WideString& src);
    WideString& operator=(const wchar* src);
    WideString& operator=(const std::wstring& src);
    WideString& operator=(wchar src);
    void attachData(WideStringData* newData);
    void replaceData(WideStringData* newData);
    WideStringData* getData() const;
    const_iterator find(const wchar* src) const;
    const_iterator find(const wchar* src, int32 length) const;
    const_iterator find(int32 start, const wchar* src) const;
    const_iterator find(int32 start, const wchar* src, int32 length) const;
    const_iterator find(const WideString& src) const;
    const_iterator find(int32 start, const WideString& src) const;
    const_iterator find(wchar ch) const;
    const_iterator find(int32 start, wchar ch) const;
    const_iterator rfind(const wchar* src) const;
    const_iterator rfind(const wchar* src, int32 length) const;
    const_iterator rfind(int32 start, const wchar* src) const;
    const_iterator rfind(int32 start, const wchar* src, int32 length) const;
    const_iterator rfind(const WideString& src) const;
    const_iterator rfind(int32 start, const WideString& src) const;
    const_iterator rfind(wchar ch) const;
    const_iterator rfind(int32 start, wchar ch) const;
    WideString concat(const wchar* src1, const wchar* src2) const;
    WideString concat(const wchar* src1, int32 src1Len, const wchar* src2, int32 src2Len) const;
    WideString concat(const WideString& src1, const wchar* src2) const;
    WideString concat(const WideString& src1, const wchar* src2, int32 src2Len) const;
    WideString concat(const wchar* src1, const WideString& src2) const;
    WideString concat(const wchar* src1, int32 src1Len, const WideString& src2) const;
    WideString concat(const WideString& src1, const WideString& src2) const;
    WideString concat(const WideString& src1, wchar ch) const;
    WideString concat(wchar ch, const WideString& src1) const;
    WideString pattern(const wchar* pattern, int32 length) const;
    WideString format(const wchar* format, ...);
    WideString vformat(const wchar* format, wchar* argList);
    void tokenize(const WideString& delimiters, std::vector<WideString>* tokenVector) const;
    void tokenize(const WideString& delimiters, std::vector<WideString>* tokenVector,
                  const wchar* delimiters2, const wchar* delimiters3, bool bKeepEmpty) const;
    void tokenizeNewLine(const WideString& delimiters, std::vector<WideString>* tokenVector) const;
    WideString join(const std::vector<WideString>& stringVector, const WideString& separator) const;
    int32 compare(const WideString& src1, const wchar* src2) const;
    int32 compare(const WideString& src1, const WideString& src2) const;
    int32 compare(const wchar* src1, const WideString& src2) const;
    int32 compareNoCase(const WideString& src1, const wchar* src2) const;
    int32 compareNoCase(const WideString& src1, const WideString& src2) const;
    int32 compareNoCase(const wchar* src1, const WideString& src2) const;
    bool isuspace(wchar ch) const;
    bool isupunct(wchar ch) const;

    wchar* buffer_;
};

char* getMbcsBuffer();
char* getTCharBuffer();
wchar* getUnicodeBuffer();
int32 getMbcsBufferSize();
int32 getTCharBufferSize();
int32 getUnicodeBufferSize();

CharString left(const CharString& str, int32 length);
CharString mid(const CharString& str, int32 start);
CharString mid(const CharString& str, int32 start, int32 length);
CharString right(const CharString& str, int32 length);
CharString trim(const CharString& str);
CharString trimLeft(const CharString& str);
CharString trimRight(const CharString& str);
CharString lower(const CharString& str);
CharString upper(const CharString& str);
CharString reverse(const CharString& str);
CharString insert(const CharString& str, int32 pos, const char* src);
CharString insert(const CharString& str, int32 pos, const char* src, int32 srcLength);
CharString insert(const CharString& str, int32 pos, const CharString& src);
CharString insert(const CharString& str, int32 pos, char src);
CharString remove(const CharString& str, int32 pos, int32 removeLength);
CharString replace(const CharString& str, const char* oldChars, const char* newChars);
CharString replace(const CharString& str, const char* oldChars, int32 oldCharsLength,
                   const char* newChars, int32 newCharsLength);
CharString replace(const CharString& str, const CharString& oldChars, const CharString& newChars);
CharString replace(const CharString& str, char oldChar, char newChar);
CharString operator+(const CharString& src1, const char* src2);
CharString operator+(const CharString& src1, const CharString& src2);
CharString operator+(const CharString& src1, char src2);
CharString operator+(const char* src1, const CharString& src2);
CharString operator+(char src1, const CharString& src2);
bool operator==(const CharString& src1, const char* src2);
bool operator==(const CharString& src1, const CharString& src2);
bool operator==(const char* src1, const CharString& src2);
bool operator!=(const CharString& src1, const char* src2);
bool operator!=(const CharString& src1, const CharString& src2);
bool operator!=(const char* src1, const CharString& src2);
bool operator<(const CharString& src1, const char* src2);
bool operator<(const CharString& src1, const CharString& src2);
bool operator<(const char* src1, const CharString& src2);
bool operator<=(const CharString& src1, const char* src2);
bool operator<=(const CharString& src1, const CharString& src2);
bool operator<=(const char* src1, const CharString& src2);
bool operator>(const CharString& src1, const char* src2);
bool operator>(const CharString& src1, const CharString& src2);
bool operator>(const char* src1, const CharString& src2);
bool operator>=(const CharString& src1, const char* src2);
bool operator>=(const CharString& src1, const CharString& src2);
bool operator>=(const char* src1, const CharString& src2);

WideString left(const WideString& str, int32 length);
WideString mid(const WideString& str, int32 start);
WideString mid(const WideString& str, int32 start, int32 length);
WideString right(const WideString& str, int32 length);
WideString trim(const WideString& str);
WideString trimLeft(const WideString& str);
WideString trimRight(const WideString& str);
WideString lower(const WideString& str);
WideString upper(const WideString& str);
WideString reverse(const WideString& str);
WideString insert(const WideString& str, int32 pos, const wchar* src);
WideString insert(const WideString& str, int32 pos, const wchar* src, int32 srcLength);
WideString insert(const WideString& str, int32 pos, const WideString& src);
WideString insert(const WideString& str, int32 pos, wchar src);
WideString remove(const WideString& str, int32 pos, int32 removeLength);
WideString replace(const WideString& str, const wchar* oldChars, const wchar* newChars);
WideString replace(const WideString& str, const wchar* oldChars, int32 oldCharsLength,
                   const wchar* newChars, int32 newCharsLength);
WideString replace(const WideString& str, const WideString& oldChars, const WideString& newChars);
WideString replace(const WideString& str, wchar oldChar, wchar newChar);
WideString operator+(const WideString& src1, const wchar* src2);
WideString operator+(const WideString& src1, const WideString& src2);
WideString operator+(const WideString& src1, wchar src2);
WideString operator+(const wchar* src1, const WideString& src2);
WideString operator+(wchar src1, const WideString& src2);
bool operator==(const WideString& src1, const wchar* src2);
bool operator==(const WideString& src1, const WideString& src2);
bool operator==(const wchar* src1, const WideString& src2);
bool operator!=(const WideString& src1, const wchar* src2);
bool operator!=(const WideString& src1, const WideString& src2);
bool operator!=(const wchar* src1, const WideString& src2);
bool operator<(const WideString& src1, const wchar* src2);
bool operator<(const WideString& src1, const WideString& src2);
bool operator<(const wchar* src1, const WideString& src2);
bool operator<=(const WideString& src1, const wchar* src2);
bool operator<=(const WideString& src1, const WideString& src2);
bool operator<=(const wchar* src1, const WideString& src2);
bool operator>(const WideString& src1, const wchar* src2);
bool operator>(const WideString& src1, const WideString& src2);
bool operator>(const wchar* src1, const WideString& src2);
bool operator>=(const WideString& src1, const wchar* src2);
bool operator>=(const WideString& src1, const WideString& src2);
bool operator>=(const wchar* src1, const WideString& src2);

#endif // STRINGS_H_
