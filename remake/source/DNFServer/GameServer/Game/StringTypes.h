#ifndef DNF_GAME_STRING_TYPES_H_
#define DNF_GAME_STRING_TYPES_H_

// ============================================================================
// df_game_r 还原 —— CharString / WideString（引用计数共享缓冲字符串）
// 布局依据 docs/class_func_reports/{StringData,CharStringData,WideStringData,
// CharString,WideString} 各报告：
//   StringData       { int refCount_;(+0x00) int size_;(+0x04); 缓冲在 +0x08 }
//   CharStringData   派生为空（getBuffer=this+8，getLength=size-1）
//   WideStringData   同上（getLength=(unsigned)size>>2 - 1）
//   CharString/WideString 仅一个指针成员 buffer_，指向数据块 +0x08 处；
//   getData() = buffer_ - 8。空串共享静态 s_empty*Data（refCount 初值 1）。
// ============================================================================

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <vector>

class StringData
{
public:
    int m_refCount;  // +0x00
    int m_size;      // +0x04（含结尾 0 的字节数 / 宽字符 x4）

    static StringData* create(int size);
    void incRef();
    void decRef();
    bool isUnique() const;
    bool isShared() const;
    int getSize() const;
    unsigned char* getBuffer();  // (unsigned char*)(this + 1)
};

class CharStringData : public StringData
{
public:
    static CharStringData* create(int length);
    static CharStringData* createTerminated(int length);
    char* getBuffer();
    int getLength() const;
};

class WideStringData : public StringData
{
public:
    static WideStringData* create(int length);
    static WideStringData* createTerminated(int length);
    wchar_t* getBuffer();
    int getLength() const;
};

class CharString
{
public:
    typedef const char* const_iterator;

    CharString();
    CharString(const CharString& src);
    CharString(const char* src);
    CharString(const char* src, int length);
    CharString(const std::string& src);
    CharString(char src);
    CharString(CharStringData* newData);
    ~CharString();

    const char* c_str() const;
    int length() const;
    bool isEmpty() const;
    bool isWhiteSpace() const;
    unsigned int getHash() const;
    char getAt(int idx) const;
    CharString setAt(int idx, char ch);
    bool startsWith(const char* pat, bool isIgnoreCase) const;
    bool endsWith(const char* pat, bool isIgnoreCase) const;
    char front() const;
    char back() const;
    const_iterator begin() const;
    const_iterator end() const;

    void assign(const CharString& src);
    void assign(const char* src);
    void assign(const char* src, int srcLength);
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

    int find(const char* pat) const;
    int find(const char* pat, int patLength) const;
    int find(int pos, const char* pat) const;
    int find(int pos, const char* pat, int patLength) const;
    int find(const CharString& pat) const;
    int find(int pos, const CharString& pat) const;
    int find(char pat) const;
    int find(int pos, char pat) const;

    int rfind(const char* pat) const;
    int rfind(const char* pat, int patLength) const;
    int rfind(int pos, const char* pat) const;
    int rfind(int pos, const char* pat, int patLength) const;
    int rfind(const CharString& pat) const;
    int rfind(int pos, const CharString& pat) const;
    int rfind(char pat) const;
    int rfind(int pos, char pat) const;

    // ORIG 全部为 static 工厂：结果构造于 sret 返回槽。
    static CharString concat(const char* src1, const char* src2);
    static CharString concat(const char* src1, int src1Len,
                             const char* src2, int src2Len);
    static CharString concat(const CharString& src1, const char* src2);
    static CharString concat(const CharString& src1, const char* src2, int src2Len);
    static CharString concat(const char* src1, const CharString& src2);
    static CharString concat(const char* src1, int src1Len, const CharString& src2);
    static CharString concat(const CharString& src1, const CharString& src2);
    static CharString concat(const CharString& src1, char ch);
    static CharString concat(char ch, const CharString& src1);

    static CharString pattern(const char* pattern, int count);
    static CharString format(const char* formatStr, ...);
    static CharString vformat(const char* formatStr, va_list argList);

    static int tokenize(const CharString& str, std::vector<CharString>* tokenVector);
    static int tokenize(const CharString& str, std::vector<CharString>* tokenVector,
                        const char* dropDelimiters, const char* returnDelimiters,
                        bool isKeepEmptyToken);
    static int tokenizeNewLine(const CharString& str,
                               std::vector<CharString>* tokenVector);
    static CharString join(const std::vector<CharString>& stringVector,
                           const CharString& separator);

    static int compare(const CharString& src1, const char* src2);
    static int compare(const CharString& src1, const CharString& src2);
    static int compare(const char* src1, const CharString& src2);
    static int compareNoCase(const CharString& src1, const char* src2);
    static int compareNoCase(const CharString& src1, const CharString& src2);
    static int compareNoCase(const char* src1, const CharString& src2);

    char* buffer_;
};

class WideString
{
public:
    typedef const wchar_t* const_iterator;

    WideString();
    WideString(const WideString& src);
    WideString(const wchar_t* src);
    WideString(const wchar_t* src, int length);
    WideString(const std::wstring& src);
    WideString(wchar_t src);
    WideString(WideStringData* newData);
    ~WideString();

    const wchar_t* c_str() const;
    int length() const;
    bool isEmpty() const;
    bool isWhiteSpace() const;
    unsigned int getHash() const;
    wchar_t getAt(int idx) const;
    WideString setAt(int idx, wchar_t ch);
    bool startsWith(const wchar_t* pat, bool isIgnoreCase) const;
    bool endsWith(const wchar_t* pat, bool isIgnoreCase) const;
    wchar_t front() const;
    wchar_t back() const;
    const_iterator begin() const;
    const_iterator end() const;

    void assign(const WideString& src);
    void assign(const wchar_t* src);
    void assign(const wchar_t* src, int srcLength);
    void assign(wchar_t src);
    void clear();

    wchar_t operator[](int idx) const;
    operator const wchar_t*() const;
    WideString& operator=(const WideString& src);
    WideString& operator=(const wchar_t* src);
    WideString& operator=(const std::wstring& src);
    WideString& operator=(wchar_t src);

    void attachData(WideStringData* newData);
    void replaceData(WideStringData* newData);
    WideStringData* getData() const;

    int find(const wchar_t* pat) const;
    int find(const wchar_t* pat, int patLength) const;
    int find(int pos, const wchar_t* pat) const;
    int find(int pos, const wchar_t* pat, int patLength) const;
    int find(const WideString& pat) const;
    int find(int pos, const WideString& pat) const;
    int find(wchar_t pat) const;
    int find(int pos, wchar_t pat) const;

    int rfind(const wchar_t* pat) const;
    int rfind(const wchar_t* pat, int patLength) const;
    int rfind(int pos, const wchar_t* pat) const;
    int rfind(int pos, const wchar_t* pat, int patLength) const;
    int rfind(const WideString& pat) const;
    int rfind(int pos, const WideString& pat) const;
    int rfind(wchar_t pat) const;
    int rfind(int pos, wchar_t pat) const;

    static WideString concat(const wchar_t* src1, const wchar_t* src2);
    static WideString concat(const wchar_t* src1, int src1Len,
                             const wchar_t* src2, int src2Len);
    static WideString concat(const WideString& src1, const wchar_t* src2);
    static WideString concat(const WideString& src1, const wchar_t* src2, int src2Len);
    static WideString concat(const wchar_t* src1, const WideString& src2);
    static WideString concat(const wchar_t* src1, int src1Len, const WideString& src2);
    static WideString concat(const WideString& src1, const WideString& src2);
    static WideString concat(const WideString& src1, wchar_t ch);
    static WideString concat(wchar_t ch, const WideString& src1);

    static WideString pattern(const wchar_t* pattern, int count);
    static WideString format(const wchar_t* formatStr, ...);
    static WideString vformat(const wchar_t* formatStr, va_list argList);

    static int tokenize(const WideString& str, std::vector<WideString>* tokenVector);
    static int tokenize(const WideString& str, std::vector<WideString>* tokenVector,
                        const wchar_t* dropDelimiters, const wchar_t* returnDelimiters,
                        bool isKeepEmptyToken);
    static int tokenizeNewLine(const WideString& str,
                               std::vector<WideString>* tokenVector);
    static WideString join(const std::vector<WideString>& stringVector,
                           const WideString& separator);

    static int compare(const WideString& src1, const wchar_t* src2);
    static int compare(const WideString& src1, const WideString& src2);
    static int compare(const wchar_t* src1, const WideString& src2);
    static int compareNoCase(const WideString& src1, const wchar_t* src2);
    static int compareNoCase(const WideString& src1, const WideString& src2);
    static int compareNoCase(const wchar_t* src1, const WideString& src2);

    static bool isuspace(wchar_t ch);
    static bool isupunct(wchar_t ch);

    wchar_t* buffer_;
};

// ---- <global> 字符串自由函数（docs/class_func_reports/_lt_global_gt_/）----
CharString left(const CharString& str, int length);
CharString mid(const CharString& str, int start);
CharString mid(const CharString& str, int start, int length);
CharString right(const CharString& str, int length);
WideString left(const WideString& str, int length);
WideString mid(const WideString& str, int start);
WideString mid(const WideString& str, int start, int length);
WideString right(const WideString& str, int length);

// ---- CharString / WideString 自由运算符（ORIG 0x8ad959a..0x8ad997c /
// 0x08adc690..0x08adca62，仅 ORIG 符号缺实现时的 out-of-line 强符号）----
bool operator==(const CharString& src1, const char* src2);
bool operator==(char const* src1, const CharString& src2);
bool operator==(const CharString& src1, const CharString& src2);
bool operator!=(const CharString& src1, const char* src2);
bool operator!=(char const* src1, const CharString& src2);
bool operator!=(const CharString& src1, const CharString& src2);
bool operator<(const CharString& src1, const char* src2);
bool operator<(char const* src1, const CharString& src2);
bool operator<(const CharString& src1, const CharString& src2);
bool operator<=(const CharString& src1, const char* src2);
bool operator<=(char const* src1, const CharString& src2);
bool operator<=(const CharString& src1, const CharString& src2);
bool operator>(const CharString& src1, const char* src2);
bool operator>(char const* src1, const CharString& src2);
bool operator>(const CharString& src1, const CharString& src2);
bool operator>=(const CharString& src1, const char* src2);
bool operator>=(char const* src1, const CharString& src2);
bool operator>=(const CharString& src1, const CharString& src2);
CharString operator+(const CharString& src1, const char* src2);
CharString operator+(const CharString& src1, const CharString& src2);
CharString operator+(const CharString& src1, char ch);
CharString operator+(const char* src1, const CharString& src2);
CharString operator+(char ch, const CharString& src2);
bool operator==(const WideString& src1, const wchar_t* src2);
bool operator==(wchar_t const* src1, const WideString& src2);
bool operator==(const WideString& src1, const WideString& src2);
bool operator!=(const WideString& src1, const wchar_t* src2);
bool operator!=(wchar_t const* src1, const WideString& src2);
bool operator!=(const WideString& src1, const WideString& src2);
bool operator<(const WideString& src1, const wchar_t* src2);
bool operator<(wchar_t const* src1, const WideString& src2);
bool operator<(const WideString& src1, const WideString& src2);
bool operator<=(const WideString& src1, const wchar_t* src2);
bool operator<=(wchar_t const* src1, const WideString& src2);
bool operator<=(const WideString& src1, const WideString& src2);
bool operator>(const WideString& src1, const wchar_t* src2);
bool operator>(wchar_t const* src1, const WideString& src2);
bool operator>(const WideString& src1, const WideString& src2);
bool operator>=(const WideString& src1, const wchar_t* src2);
bool operator>=(wchar_t const* src1, const WideString& src2);
bool operator>=(const WideString& src1, const WideString& src2);
WideString operator+(const WideString& src1, const wchar_t* src2);
WideString operator+(const WideString& src1, const WideString& src2);
WideString operator+(const WideString& src1, wchar_t ch);
WideString operator+(const wchar_t* src1, const WideString& src2);
WideString operator+(wchar_t ch, const WideString& src2);

// ---- CharString / WideString 自由 insert/replace（G4 域，StringConvert.cpp；
//      ORIG 0x08ad76ce..0x08adae9d，sret 返回）----
CharString insert(const CharString& str, int pos, const char* src);
CharString insert(const CharString& str, int pos, const char* src, int srcLength);
CharString insert(const CharString& str, int pos, const CharString& src);
CharString insert(const CharString& str, int pos, char src);
WideString insert(const WideString& str, int pos, const wchar_t* src);
WideString insert(const WideString& str, int pos, const wchar_t* src, int srcLength);
WideString insert(const WideString& str, int pos, const WideString& src);
WideString insert(const WideString& str, int pos, wchar_t src);
CharString replace(const CharString& str, const char* pat, const char* target);
CharString replace(const CharString& str, const char* pat, int patLength,
                   const char* target, int targetLength);
CharString replace(const CharString& str, const CharString& pat, const CharString& target);
CharString replace(const CharString& str, char pat, char target);
WideString replace(const WideString& str, const wchar_t* pat, const wchar_t* target);
WideString replace(const WideString& str, const wchar_t* pat, int patLength,
                   const wchar_t* target, int targetLength);
WideString replace(const WideString& str, const WideString& pat, const WideString& target);
WideString replace(const WideString& str, wchar_t pat, wchar_t target);

// ---- 宽字符转换自由函数（G4 域；toUnicode 返回 TLS 环形缓冲槽指针）----
const wchar_t* toUnicode(const char* src);
const wchar_t* toUnicode(const wchar_t* src);
const wchar_t* toUnicode(const std::string& src);
const wchar_t* toUnicode(const std::wstring& src);
void toUnicode(const char* src, std::wstring& dst);
void toUnicode(const wchar_t* src, std::wstring& dst);
void toUnicode(const std::string& src, std::wstring& dst);
void toUnicode(const std::wstring& src, std::wstring& dst);

WideString convertToWideString(const char* src);
WideString convertToWideString(const char* src, unsigned int codePage);
// ---- 大小写/裁剪/反转/删除自由函数（OnlyOrigBatch7；实现 StringCaseTools.cpp）----
CharString lower(const CharString& str);
WideString lower(const WideString& str);
CharString upper(const CharString& str);
WideString upper(const WideString& str);
CharString reverse(const CharString& str);
WideString reverse(const WideString& str);
CharString remove(const CharString& str, int pos, int removeLength);
WideString remove(const WideString& str, int pos, int removeLength);
CharString trim(const CharString& str);
WideString trim(const WideString& str);
CharString trimLeft(const CharString& str);
WideString trimLeft(const WideString& str);
CharString trimRight(const CharString& str);
WideString trimRight(const WideString& str);

// ---- 缓冲读取（2 字节长度前缀；实现 StringCaseTools.cpp）----
void readStringFromBuffer(char* dst, int maxLen, char*& buf);
void readStringFromBuffer(std::string& str, char*& buf);

// ORIG T 0x08ad457c / 0x08ad45f3（mangled 第 4 参为 char*，ABI 上 va_list 即 char*）。
int ss_vsprintf(char* buffer, unsigned int count, const char* format, char* ap);
int ss_vswprintf(wchar_t* buffer, unsigned int count, const wchar_t* format, char* ap);

#endif  // DNF_GAME_STRING_TYPES_H_
