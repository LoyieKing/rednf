// df_game_r 还原 —— CharString / WideString 实现
// 依据 docs/class_func_reports/{StringData,CharStringData,WideStringData,
// CharString,WideString,_lt_global_gt_(left/right/mid/ss_vsprintf)} 各报告逐函数转写。
#include "StringTypes.h"
#include <new>   // placement new：vformat 就地覆盖重建（ORIG this->C1 形，4.4 禁 p->Class::Class()）

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// ============================================================================
// 空串共享数据。ORIG df_game_r .data 布局：
//   0x93f5688 s_emptyCharStringDataRaw  { refCount=1, size=1, buf='\0' }
//   0x93f5694 s_emptyCharStringData     -> raw 指针
//   0x93f5698 s_emptyWideStringDataRaw  { refCount=1, size=4, buf=L'\0' }
//   0x93f56a4 s_emptyWideStringData     -> raw 指针
// 初值 refCount=1：每个空串 attach 时 incRef，析构 decRef 后仍 >=1，永不释放。
// ============================================================================
struct EmptyCharStringDataStorage
{
    int m_refCount;
    int m_size;
    char m_buffer;
};
static EmptyCharStringDataStorage s_emptyCharStringDataRaw = {1, 1, 0};
static CharStringData* s_emptyCharStringData =
    reinterpret_cast<CharStringData*>(&s_emptyCharStringDataRaw);

struct EmptyWideStringDataStorage
{
    int m_refCount;
    int m_size;
    wchar_t m_buffer;
};
static EmptyWideStringDataStorage s_emptyWideStringDataRaw = {1, 4, 0};
static WideStringData* s_emptyWideStringData =
    reinterpret_cast<WideStringData*>(&s_emptyWideStringDataRaw);

// ---- StringData（docs/class_func_reports/StringData/）----

void StringData::incRef()
{
    // ORIG: lock addl $0x1,(%eax)
    __sync_fetch_and_add(&m_refCount, 1);
}

void StringData::decRef()
{
    // ORIG: lock xadd -1；新值 <=0 则 free(this)
    if (__sync_sub_and_fetch(&m_refCount, 1) <= 0)
        ::free(this);
}

bool StringData::isUnique() const
{
    return m_refCount == 1;
}

bool StringData::isShared() const
{
    return m_refCount > 1;
}

int StringData::getSize() const
{
    return m_size;
}

unsigned char* StringData::getBuffer()
{
    return reinterpret_cast<unsigned char*>(this + 1);
}

StringData* StringData::create(int size)
{
    StringData* data = static_cast<StringData*>(malloc(size + 8));
    if (data == 0)
        return 0;
    data->m_refCount = 0;
    data->m_size = size;
    return data;
}

// ---- CharStringData ----

int CharStringData::getLength() const
{
    return m_size - 1;
}

char* CharStringData::getBuffer()
{
    return reinterpret_cast<char*>(this + 1);
}

CharStringData* CharStringData::create(int length)
{
    int size = length + 1;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<CharStringData*>(data);
}

CharStringData* CharStringData::createTerminated(int length)
{
    CharStringData* data = create(length);
    data->getBuffer()[length] = '\0';
    return data;
}

// ---- WideStringData ----

int WideStringData::getLength() const
{
    return static_cast<int>(static_cast<unsigned int>(m_size) >> 2) - 1;
}

wchar_t* WideStringData::getBuffer()
{
    return reinterpret_cast<wchar_t*>(this + 1);
}

WideStringData* WideStringData::create(int length)
{
    int size = (length + 1) * 4;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<WideStringData*>(data);
}

WideStringData* WideStringData::createTerminated(int length)
{
    WideStringData* data = create(length);
    data->getBuffer()[length] = 0;
    return data;
}

// ---- CharString 生命周期与数据访问 ----

CharString::CharString()
{
    attachData(s_emptyCharStringData);
}

CharString::CharString(const CharString& src)
{
    CharStringData* newData = src.getData();
    attachData(newData);
}

CharString::CharString(const char* src)
{
    int length = static_cast<int>(strlen(src));
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::create(length);
        memcpy(data->getBuffer(), src, length + 1);
        attachData(data);
    }
}

CharString::CharString(const char* src, int length)
{
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::createTerminated(length);
        memcpy(data->getBuffer(), src, length);
        attachData(data);
    }
}

CharString::CharString(const std::string& src)
{
    int length = static_cast<int>(src.size());
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::create(length);
        memcpy(data->getBuffer(), src.c_str(), length + 1);
        attachData(data);
    }
}

CharString::CharString(char src)
{
    CharStringData* data = CharStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

CharString::CharString(CharStringData* newData)
{
    attachData(newData);
}

CharString::~CharString()
{
    getData()->decRef();
}

const char* CharString::c_str() const
{
    return buffer_;
}

int CharString::length() const
{
    return getData()->getLength();
}

bool CharString::isEmpty() const
{
    return length() == 0;
}

bool CharString::isWhiteSpace() const
{
    const char* buf = buffer_;
    while (*buf != '\0')
    {
        if (isspace(static_cast<int>(*buf)) == 0)
            return false;
        ++buf;
    }
    return true;
}

unsigned int CharString::getHash() const
{
    // [b28d-34] AE（compare_tu 口径，ndiff 2→0）：残差仅 4.4 循环出口 nop 填充
    // （4.1 无；-falign-loops=1/-fno-align-jumps 实测无效）⇒ §6.1 对齐 nop 豁免；
    // 归一后逐条全等。ORIG 0x08ad6c1c。
    // ORIG：isEmpty 结果直接 test %al（bool 返回值不物化），空串走 mov $0,%eax
    // 常量岛提前 return；value=0 初始化在检查之后落槽。
    if (isEmpty())
        return 0;
    unsigned int value = 0;
    const char* str = buffer_;
    while (true)
    {
        char c = *str;
        str = str + 1;
        if (c == '\0')
            break;
        value = ((value << 6) + (value << 16)) - value + static_cast<int>(c);
    }
    if (value == 0)
        value = 1;
    return value;
}

char CharString::getAt(int idx) const
{
    return buffer_[idx];
}

CharString CharString::setAt(int idx, char ch)
{
    // [b28d-34] 复核维持 DIFF（N82 族）：ORIG 0x08ad6ca2 用 edi/esi 跨调用缓存
    // 且帧 0x2c；4.4 帧 0x20 并多一次 getData/create 调用序，源形不可择
    // （P0/P183 表达式化已试）；语义等价。
    CharStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        CharStringData* thisData = getData();
        newData = CharStringData::create(thisData->getLength());
        char* dest = newData->getBuffer();
        memcpy(dest, thisData->getBuffer(), thisData->getLength() + 1);
        newData->getBuffer()[idx] = ch;
        return CharString(newData);
    }
}

bool CharString::startsWith(const char* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(strlen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str(), pat, patLength) == 0;
    else
        return strncmp(c_str(), pat, patLength) == 0;
}

bool CharString::endsWith(const char* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(strlen(pat));
    // [b28d-34] ORIG 0x08ad9xxx：`cmp; setl %al; test %al; je body; mov$0,%eax;
    // jmp end`（tooShort 早退岛，无槽 ⇒ 原码无 bool 局部）。早退 return false 形。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str() + (length() - patLength), pat, patLength) == 0;
    return strncmp(c_str() + (length() - patLength), pat, patLength) == 0;
}

char CharString::front() const
{
    return *buffer_;
}

char CharString::back() const
{
    // [b28c-16 P0 清绑 + N96] 原 register asm("eax") 绑定违反 P0 全域禁则，
    // 已按 P0 清除。自然形（`if(len())`/`!=0`/`>0`/cast，探针 p9 b1..b5）
    // 4.4 恒多 `setcc %al; test %al,%al` 两条 bool 物化 vs ORIG 单条
    // `test %eax,%eax; je` ⇒ 落 N96（绑定赋值中转）+P168a④。
    // [b28d-34] 复核维持 N96/P168a④（ORIG 0x08ad6edc）：4.4 对 int callee 条件
    // 调用恒物化 setne+test，源形五探针全败；语义等价。
    if (length())
        return buffer_[length() - 1];
    return '\0';
}

CharString::const_iterator CharString::begin() const
{
    return buffer_;
}

CharString::const_iterator CharString::end() const
{
    return buffer_ + length();
}

void CharString::assign(const CharString& src)
{
    // ORIG 0x08ad6fed：`cmp %eax,%ebx; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223）。while+break 空体被 4.4 折掉 jne（实测 -3 条）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
}

void CharString::assign(const char* src)
{
    assign(src, static_cast<int>(strlen(src)));
}

void CharString::assign(const char* src, int srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength);
        replaceData(newData);
    }
}

void CharString::assign(char src)
{
    if (src == '\0')
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
}

void CharString::clear()
{
    // ORIG 0x08ad707f：`test %al,%al; jne +0x2b` + 体尾 `jmp +0x2c` + `nop`
    // ⇒ P223 goto 尾空标签复现 4.1 空 then 占位岛。
    if (isEmpty())
        goto done;
    replaceData(s_emptyCharStringData);
done:;
}

char CharString::operator[](int idx) const
{
    return getAt(idx);
}

CharString::operator const char*() const
{
    return buffer_;
}

CharString& CharString::operator=(const CharString& src)
{
    assign(src);
    return *this;
}

CharString& CharString::operator=(const char* src)
{
    assign(src);
    return *this;
}

CharString& CharString::operator=(const std::string& src)
{
    assign(src.c_str(), static_cast<int>(src.size()));
    return *this;
}

CharString& CharString::operator=(char src)
{
    assign(src);
    return *this;
}

void CharString::attachData(CharStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

void CharString::replaceData(CharStringData* newData)
{
    CharStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

CharStringData* CharString::getData() const
{
    return reinterpret_cast<CharStringData*>(buffer_ - 8);
}

// ---- CharString::find / rfind ----

int CharString::find(const char* pat) const
{
    return find(0, pat, static_cast<int>(strlen(pat)));
}

int CharString::find(int pos, const char* pat) const
{
    return find(pos, pat, static_cast<int>(strlen(pat)));
}

int CharString::find(const char* pat, int patLength) const
{
    return find(0, pat, patLength);
}

int CharString::find(int pos, const char* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const char* strBuf = c_str();
    int endPos = length() - patLength;
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength) == 0)
            return thisPos;
    }
    return -1;
}

int CharString::find(const CharString& pat) const
{
    return find(0, pat.c_str(), pat.length());
}

int CharString::find(int pos, const CharString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
}

int CharString::find(char pat) const
{
    return find(0, pat);
}

int CharString::find(int pos, char pat) const
{
    if (pat == '\0')
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const char* strBuf = c_str();
    int endPos = length() - 1;
    // 注意：ORIG 此处固定用 strBuf[pos] 比较（保留原始行为）。
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (strBuf[pos] == pat)
            return thisPos;
    }
    return -1;
}

int CharString::rfind(const char* pat) const
{
    int patLength = static_cast<int>(strlen(pat));
    return rfind(length() - patLength, pat, patLength);
}

int CharString::rfind(int pos, const char* pat) const
{
    return rfind(pos, pat, static_cast<int>(strlen(pat)));
}

int CharString::rfind(const char* pat, int patLength) const
{
    return rfind(length() - patLength, pat, patLength);
}

int CharString::rfind(int pos, const char* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const char* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength) == 0)
            return thisPos;
    }
    return -1;
}

int CharString::rfind(const CharString& pat) const
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

int CharString::rfind(int pos, const CharString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
}

int CharString::rfind(char pat) const
{
    return rfind(length() - 1, pat);
}

int CharString::rfind(int pos, char pat) const
{
    if (pat == '\0')
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const char* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
            return thisPos;
    }
    return -1;
}

// ---- CharString static 工厂 ----

CharString CharString::concat(const char* src1, const char* src2)
{
    return concat(src1, static_cast<int>(strlen(src1)),
                  src2, static_cast<int>(strlen(src2)));
}

CharString CharString::concat(const char* src1, int src1Len,
                              const char* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len);
    memcpy(newData->getBuffer() + src1Len, src2, src2Len);
    return CharString(newData);
}

CharString CharString::concat(const CharString& src1, const char* src2)
{
    return concat(src1, src2, static_cast<int>(strlen(src2)));
}

CharString CharString::concat(const CharString& src1, const char* src2, int src2Len)
{
    if (src1.length() == 0)
        return CharString(src2, src2Len);
    if (src2Len == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
}

CharString CharString::concat(const char* src1, const CharString& src2)
{
    return concat(src1, static_cast<int>(strlen(src1)), src2);
}

CharString CharString::concat(const char* src1, int src1Len, const CharString& src2)
{
    if (src1Len == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
}

CharString CharString::concat(const CharString& src1, const CharString& src2)
{
    if (src1.length() == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
}

CharString CharString::concat(const CharString& src1, char ch)
{
    if (src1.length() == 0)
        return CharString(ch);
    if (ch == '\0')
        return CharString(src1);
    CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
    memcpy(newData->getBuffer(), src1.c_str(), src1.length());
    newData->getBuffer()[src1.length()] = ch;
    return CharString(newData);
}

CharString CharString::concat(char ch, const CharString& src1)
{
    if (ch == '\0')
        return CharString(src1);
    if (src1.length() == 0)
        return CharString(ch);
    CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
    newData->getBuffer()[0] = ch;
    memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length());
    return CharString(newData);
}

CharString CharString::pattern(const char* pattern, int count)
{
    int patternLen = static_cast<int>(strlen(pattern));
    if (patternLen == 0 || count < 1)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(patternLen * count);
    char* buffer = newData->getBuffer();
    for (int i = 0; i < count; ++i)
    {
        memcpy(buffer, pattern, patternLen);
        buffer += patternLen;
    }
    return CharString(newData);
}

CharString& CharString::format(const char* formatStr, ...)
{
    va_list argList;
    va_start(argList, formatStr);
    // N 封顶注：ORIG 0x08ad8afa 体内联 vsnprintf+双 placement-C1 直发形依赖 4.1.2 非平凡
    // union 扩展（单条 call C1Ev，零 operator-new 噪声）；4.4.7 拒 union 且 placement new
    // 经本类 operator new[](size_t, void*) 产 `_ZnwjPv`+null 检查双噪声（实测 61 vs 53）。
    // 委托 vformat=语义等价最小形；字节级同形属类层专批（b25 trivial 化）一并解。
    // [b28d-34] 复核维持 N83（ORIG 0x08ad8afa）：4.4 placement operator new 恒发
    // _ZnwjPv call+size 装载行+EH 段（内联展开实测 61 vs ORIG 53），封顶。
    this->vformat(formatStr, argList);
    va_end(argList);
    return *this;
}

// ORIG T 0x08ad8b9e：length==0 → this->C1Ev()（**就地默认构造覆盖**，旧 data 不 detach——
// ORIG 体实证无 ~CharString/D1 调用）；否则 create(length)+ss_vsprintf 后 this->C1EP14CharStringData
// 就地覆盖；尾 return *this。
// [b28d-34] 复核维持 N83：ORIG 内联 placement new（0 call）；4.4 发 _ZnwjPv call+
// 判空+EH 残段（58 vs 49），源形不可控。
CharString& CharString::vformat(const char* formatStr, va_list argList)
{
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        new (this) CharString();
        return *this;
    }
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    new (this) CharString(newData);
    return *this;
}

int CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector)
{
    return tokenize(str, tokenVector, " \t\n\r", ".,", false);
}

int CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector,
                         const char* dropDelimiters, const char* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x08ad8c6c 循环出口岛/落点与 4.4 循环旋转
    // 布局差（clean 269=269，仅岛位置不同）；N82/P164 族，语义等价。
    const char* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        char c = strBuf[i];
        const char* d0 = dropDelimiters;
        while (*d0 != '\0' && *d0 != c)
            ++d0;
        if (*d0 == '\0')
        {
            const char* d1 = returnDelimiters;
            while (*d1 != '\0' && *d1 != c)
                ++d1;
            if (*d1 != '\0')
            {
                int newLen = i - nextStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(CharString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(CharString(*d1));
                tokenCount = tokenCount + 1;
                nextStartIdx = i + 1;
            }
        }
        else
        {
            int newLen = i - nextStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(CharString());
                    tokenCount = tokenCount + 1;
                }
            }
            else
            {
                tokenVector->push_back(mid(str, nextStartIdx, newLen));
                tokenCount = tokenCount + 1;
            }
            nextStartIdx = i + 1;
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

int CharString::tokenizeNewLine(const CharString& str,
                                std::vector<CharString>* tokenVector)
{
    const char* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int lastNewLineIdx = -1;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        char c = strBuf[i];
        if (c == '\r')
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == '\n')
                i = i + 1;
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == '\n')
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    else if (strLen - 1 == lastNewLineIdx)
    {
        tokenVector->push_back(CharString());
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

CharString CharString::join(const std::vector<CharString>& stringVector,
                            const CharString& separator)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x08ad920e 帧 0x60 vs 4.4 0x40 + 迭代器/临时
    // 槽序差（P28/N86 族）；语义等价。
    if (stringVector.empty())
    {
        return CharString();
    }
    const char* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    for (std::vector<CharString>::const_iterator it = stringVector.begin();
         it != stringVector.end(); ++it)
    {
        totalLength += it->length();
    }
    if (totalLength == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(totalLength);
    char* newDataBuf = newData->getBuffer();
    bool tokenFirst = true;
    for (std::vector<CharString>::const_iterator it = stringVector.begin();
         it != stringVector.end(); ++it)
    {
        if (separatorStrLen >= 1 && !tokenFirst)
        {
            memcpy(newDataBuf, separatorStrSrc, separatorStrLen);
            newDataBuf += separatorStrLen;
        }
        int tokenLength = it->length();
        memcpy(newDataBuf, it->c_str(), tokenLength);
        newDataBuf += tokenLength;
        tokenFirst = false;
    }
    return CharString(newData);
}

int CharString::compare(const CharString& src1, const char* src2)
{
    return strcmp(src1.c_str(), src2);
}

int CharString::compare(const CharString& src1, const CharString& src2)
{
    return strcmp(src1.c_str(), src2.c_str());
}

int CharString::compare(const char* src1, const CharString& src2)
{
    return strcmp(src1, src2.c_str());
}

int CharString::compareNoCase(const CharString& src1, const char* src2)
{
    return strcasecmp(src1.c_str(), src2);
}

int CharString::compareNoCase(const CharString& src1, const CharString& src2)
{
    return strcasecmp(src1.c_str(), src2.c_str());
}

int CharString::compareNoCase(const char* src1, const CharString& src2)
{
    return strcasecmp(src1, src2.c_str());
}

// ---- WideString 生命周期与数据访问 ----

WideString::WideString()
{
    attachData(s_emptyWideStringData);
}

WideString::WideString(const WideString& src)
{
    WideStringData* newData = src.getData();
    attachData(newData);
}

WideString::WideString(const wchar_t* src)
{
    int length = static_cast<int>(wcslen(src));
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create(length);
        memcpy(data->getBuffer(), src, (length + 1) * sizeof(wchar_t));
        attachData(data);
    }
}

WideString::WideString(const wchar_t* src, int length)
{
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::createTerminated(length);
        memcpy(data->getBuffer(), src, length * sizeof(wchar_t));
        attachData(data);
    }
}

WideString::WideString(const std::wstring& src)
{
    int length = static_cast<int>(src.size());
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create(length);
        memcpy(data->getBuffer(), src.c_str(), (length + 1) * sizeof(wchar_t));
        attachData(data);
    }
}

WideString::WideString(wchar_t src)
{
    WideStringData* data = WideStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

WideString::WideString(WideStringData* newData)
{
    attachData(newData);
}

WideString::~WideString()
{
    getData()->decRef();
}

const wchar_t* WideString::c_str() const
{
    return buffer_;
}

int WideString::length() const
{
    return getData()->getLength();
}

bool WideString::isEmpty() const
{
    return length() == 0;
}

bool WideString::isWhiteSpace() const
{
    const wchar_t* buf = buffer_;
    while (*buf != 0)
    {
        if (!isuspace(*buf))
            return false;
        ++buf;
    }
    return true;
}

unsigned int WideString::getHash() const
{
    // [b28d-34] AE（同 CharString::getHash，nop 豁免）。ORIG 0x08ad9c74。
    // ORIG：isEmpty 结果直接 test %al（bool 返回值不物化），空串走 mov $0,%eax
    // 常量岛提前 return；value=0 初始化在检查之后落槽。
    if (isEmpty())
        return 0;
    unsigned int value = 0;
    const wchar_t* str = buffer_;
    while (true)
    {
        int c = *str;
        str = str + 1;
        if (c == 0)
            break;
        value = ((value << 6) + (value << 16)) - value + c;
    }
    if (value == 0)
        value = 1;
    return value;
}

wchar_t WideString::getAt(int idx) const
{
    return buffer_[idx];
}

WideString WideString::setAt(int idx, wchar_t ch)
{
    // [b28d-34] 复核维持 DIFF（N82 族，同 CharString::setAt）。ORIG 0x08ad9cf8。
    WideStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        WideStringData* thisData = getData();
        newData = WideStringData::create(thisData->getLength());
        wchar_t* dest = newData->getBuffer();
        memcpy(dest, thisData->getBuffer(),
               (thisData->getLength() + 1) * sizeof(wchar_t));
        newData->getBuffer()[idx] = ch;
        return WideString(newData);
    }
}

bool WideString::startsWith(const wchar_t* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(wcslen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str(), pat, patLength) == 0;
    else
        return wcsncmp(c_str(), pat, patLength) == 0;
}

bool WideString::endsWith(const wchar_t* pat, bool isIgnoreCase) const
{
    int patLength = static_cast<int>(wcslen(pat));
    // [b28d-34] 同 CharString::endsWith：早退 return false 形复现 ORIG 0x08ad9xxx
    // `setl; test; je body` + `mov$0,jmp` 岛；残差仅 `length()-patLength` 物化段
    // （N-86b，与 right 族同）。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str() + (length() - patLength), pat, patLength) == 0;
    return wcsncmp(c_str() + (length() - patLength), pat, patLength) == 0;
}

wchar_t WideString::front() const
{
    return *buffer_;
}

wchar_t WideString::back() const
{
    // [b28c-16 P0 清绑 + N96] 同上。
    // [b28d-34] 复核维持 N96/P168a④（ORIG 0x08ad9f36）。
    if (length())
        return buffer_[length() - 1];
    return 0;
}

WideString::const_iterator WideString::begin() const
{
    return buffer_;
}

WideString::const_iterator WideString::end() const
{
    return buffer_ + length();
}

void WideString::assign(const WideString& src)
{
    // ORIG 0x08ad9feb：`cmp; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223，同 CharString::assign）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
}

void WideString::assign(const wchar_t* src)
{
    assign(src, static_cast<int>(wcslen(src)));
}

void WideString::assign(const wchar_t* src, int srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength * sizeof(wchar_t));
        replaceData(newData);
    }
}

void WideString::assign(wchar_t src)
{
    if (src == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
}

void WideString::clear()
{
    // ORIG 0x08ada0dd：`test %al,%al; jne +0x2b` + `jmp +0x2c` + `nop`
    // ⇒ P223 goto 尾空标签（同 CharString::clear）。
    if (isEmpty())
        goto done;
    replaceData(s_emptyWideStringData);
done:;
}

wchar_t WideString::operator[](int idx) const
{
    return getAt(idx);
}

WideString::operator const wchar_t*() const
{
    return buffer_;
}

WideString& WideString::operator=(const WideString& src)
{
    assign(src);
    return *this;
}

WideString& WideString::operator=(const wchar_t* src)
{
    assign(src);
    return *this;
}

WideString& WideString::operator=(const std::wstring& src)
{
    assign(src.c_str(), static_cast<int>(src.size()));
    return *this;
}

WideString& WideString::operator=(wchar_t src)
{
    assign(src);
    return *this;
}

void WideString::attachData(WideStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

void WideString::replaceData(WideStringData* newData)
{
    WideStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

WideStringData* WideString::getData() const
{
    return reinterpret_cast<WideStringData*>(reinterpret_cast<char*>(buffer_) - 8);
}

// ---- WideString::find / rfind ----

int WideString::find(const wchar_t* pat) const
{
    return find(0, pat, static_cast<int>(wcslen(pat)));
}

int WideString::find(int pos, const wchar_t* pat) const
{
    return find(pos, pat, static_cast<int>(wcslen(pat)));
}

int WideString::find(const wchar_t* pat, int patLength) const
{
    return find(0, pat, patLength);
}

int WideString::find(int pos, const wchar_t* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const wchar_t* strBuf = c_str();
    int endPos = length() - patLength;
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength * sizeof(wchar_t)) == 0)
            return thisPos;
    }
    return -1;
}

int WideString::find(const WideString& pat) const
{
    return find(0, pat.c_str(), pat.length());
}

int WideString::find(int pos, const WideString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
}

int WideString::find(wchar_t pat) const
{
    return find(0, pat);
}

int WideString::find(int pos, wchar_t pat) const
{
    if (pat == 0)
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const wchar_t* strBuf = c_str();
    int endPos = length() - 1;
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (strBuf[thisPos] == pat)
            return thisPos;
    }
    return -1;
}

int WideString::rfind(const wchar_t* pat) const
{
    int patLength = static_cast<int>(wcslen(pat));
    return rfind(length() - patLength, pat, patLength);
}

int WideString::rfind(int pos, const wchar_t* pat) const
{
    return rfind(pos, pat, static_cast<int>(wcslen(pat)));
}

int WideString::rfind(const wchar_t* pat, int patLength) const
{
    return rfind(length() - patLength, pat, patLength);
}

int WideString::rfind(int pos, const wchar_t* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const wchar_t* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength * sizeof(wchar_t)) == 0)
            return thisPos;
    }
    return -1;
}

int WideString::rfind(const WideString& pat) const
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

int WideString::rfind(int pos, const WideString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
}

int WideString::rfind(wchar_t pat) const
{
    return rfind(length() - 1, pat);
}

int WideString::rfind(int pos, wchar_t pat) const
{
    if (pat == 0)
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const wchar_t* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
            return thisPos;
    }
    return -1;
}

// ---- WideString static 工厂 ----

WideString WideString::concat(const wchar_t* src1, const wchar_t* src2)
{
    return concat(src1, static_cast<int>(wcslen(src1)),
                  src2, static_cast<int>(wcslen(src2)));
}

WideString WideString::concat(const wchar_t* src1, int src1Len,
                              const wchar_t* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len * sizeof(wchar_t));
    memcpy(newData->getBuffer() + src1Len, src2, src2Len * sizeof(wchar_t));
    return WideString(newData);
}

WideString WideString::concat(const WideString& src1, const wchar_t* src2)
{
    return concat(src1, src2, static_cast<int>(wcslen(src2)));
}

WideString WideString::concat(const WideString& src1, const wchar_t* src2, int src2Len)
{
    if (src1.length() == 0)
        return WideString(src2, src2Len);
    if (src2Len == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
}

WideString WideString::concat(const wchar_t* src1, const WideString& src2)
{
    return concat(src1, static_cast<int>(wcslen(src1)), src2);
}

WideString WideString::concat(const wchar_t* src1, int src1Len, const WideString& src2)
{
    if (src1Len == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
}

WideString WideString::concat(const WideString& src1, const WideString& src2)
{
    if (src1.length() == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
}

WideString WideString::concat(const WideString& src1, wchar_t ch)
{
    if (src1.length() == 0)
        return WideString(ch);
    if (ch == 0)
        return WideString(src1);
    WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
    memcpy(newData->getBuffer(), src1.c_str(), src1.length() * sizeof(wchar_t));
    newData->getBuffer()[src1.length()] = ch;
    return WideString(newData);
}

WideString WideString::concat(wchar_t ch, const WideString& src1)
{
    if (ch == 0)
        return WideString(src1);
    if (src1.length() == 0)
        return WideString(ch);
    WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
    newData->getBuffer()[0] = ch;
    memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length() * sizeof(wchar_t));
    return WideString(newData);
}

WideString WideString::pattern(const wchar_t* pattern, int count)
{
    int patternLen = static_cast<int>(wcslen(pattern));
    if (patternLen == 0 || count < 1)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(patternLen * count);
    wchar_t* buffer = newData->getBuffer();
    for (int i = 0; i < count; ++i)
    {
        memcpy(buffer, pattern, patternLen * sizeof(wchar_t));
        buffer += patternLen;
    }
    return WideString(newData);
}

WideString& WideString::format(const wchar_t* formatStr, ...)
{
    va_list argList;
    va_start(argList, formatStr);
    this->vformat(formatStr, argList);   // CharString::format 同案（N 封顶，b25 类层批）
    // [b28d-34] 复核维持 N83（ORIG 0x08adbbe4，同 CharString::format）。
    va_end(argList);
    return *this;
}

// CharString::vformat 同形（ORIG T 0x08adbc88）。
// [b28d-34] 复核维持 N83（同 CharString::vformat）。
WideString& WideString::vformat(const wchar_t* formatStr, va_list argList)
{
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        new (this) WideString();
        return *this;
    }
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    new (this) WideString(newData);
    return *this;
}

int WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector)
{
    return tokenize(str, tokenVector, L" \t\n\r", L".", false);
}

int WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector,
                         const wchar_t* dropDelimiters, const wchar_t* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-34] 复核维持 DIFF（同 CharString::tokenize）。ORIG 0x08adbd56。
    const wchar_t* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        wchar_t c = strBuf[i];
        const wchar_t* d0 = dropDelimiters;
        while (*d0 != 0 && *d0 != c)
            ++d0;
        if (*d0 == 0)
        {
            const wchar_t* d1 = returnDelimiters;
            while (*d1 != 0 && *d1 != c)
                ++d1;
            if (*d1 != 0)
            {
                int newLen = i - nextStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(WideString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(WideString(*d1));
                tokenCount = tokenCount + 1;
                nextStartIdx = i + 1;
            }
        }
        else
        {
            int newLen = i - nextStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(WideString());
                    tokenCount = tokenCount + 1;
                }
            }
            else
            {
                tokenVector->push_back(mid(str, nextStartIdx, newLen));
                tokenCount = tokenCount + 1;
            }
            nextStartIdx = i + 1;
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

int WideString::tokenizeNewLine(const WideString& str,
                                std::vector<WideString>* tokenVector)
{
    const wchar_t* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int lastNewLineIdx = -1;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        wchar_t c = strBuf[i];
        if (c == 0xd)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == 0xa)
                i = i + 1;
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == 0xa)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    else if (strLen - 1 == lastNewLineIdx)
    {
        tokenVector->push_back(WideString());
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

WideString WideString::join(const std::vector<WideString>& stringVector,
                            const WideString& separator)
{
    // [b28d-34] 复核维持 DIFF（同 CharString::join）。ORIG 0x08adc2f4。
    if (stringVector.empty())
    {
        return WideString();
    }
    const wchar_t* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    for (std::vector<WideString>::const_iterator it = stringVector.begin();
         it != stringVector.end(); ++it)
    {
        totalLength += it->length();
    }
    if (totalLength == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(totalLength);
    wchar_t* newDataBuf = newData->getBuffer();
    bool tokenFirst = true;
    for (std::vector<WideString>::const_iterator it = stringVector.begin();
         it != stringVector.end(); ++it)
    {
        if (separatorStrLen >= 1 && !tokenFirst)
        {
            memcpy(newDataBuf, separatorStrSrc, separatorStrLen * sizeof(wchar_t));
            newDataBuf += separatorStrLen;
        }
        int tokenLength = it->length();
        memcpy(newDataBuf, it->c_str(), tokenLength * sizeof(wchar_t));
        newDataBuf += tokenLength;
        tokenFirst = false;
    }
    return WideString(newData);
}

int WideString::compare(const WideString& src1, const wchar_t* src2)
{
    return wcscmp(src1.c_str(), src2);
}

int WideString::compare(const WideString& src1, const WideString& src2)
{
    return wcscmp(src1.c_str(), src2.c_str());
}

int WideString::compare(const wchar_t* src1, const WideString& src2)
{
    return wcscmp(src1, src2.c_str());
}

int WideString::compareNoCase(const WideString& src1, const wchar_t* src2)
{
    return wcscasecmp(src1.c_str(), src2);
}

int WideString::compareNoCase(const WideString& src1, const WideString& src2)
{
    return wcscasecmp(src1.c_str(), src2.c_str());
}

int WideString::compareNoCase(const wchar_t* src1, const WideString& src2)
{
    return wcscasecmp(src1, src2.c_str());
}

bool WideString::isuspace(wchar_t ch)
{
    // ORIG 0x08adca62：Unicode 空白白名单。
    if ((((((ch == 9) || (ch == 10)) || (ch == 0xb)) ||
          (((ch == 0xc || (ch == 0xd)) || ((ch == 0x20 || ((ch == 0xa0 || (ch == 0x2000)))))))) ||
         ((ch == 0x2001 ||
          ((((((ch == 0x2002 || (ch == 0x2003)) || (ch == 0x2004)) ||
             ((ch == 0x2005 || (ch == 0x2006)))) ||
            ((ch == 0x2007 || ((ch == 0x2008 || (ch == 0x2009)))))) || (ch == 0x200a)))))) ||
        ((((ch == 0x200b || (ch == 0x202f)) || (ch == 0x205f)) ||
          ((ch == 0x3000 || (ch == 0xfeff))))))
    {
        return true;
    }
    return false;
}

bool WideString::isupunct(wchar_t ch)
{
    // ORIG 0x08adcb50：条件为「不是 Unicode 标点」，命中返回 false，否则 true。
    // 极性反演（P196）：ORIG true 常量岛在前（mov $1;jmp 在 mov $0 之前），
    // 需 if 条件 = 「是标点」命中 → return true。
    if (!(((((((((((ch < 0x21) || (0x23 < ch)) && ((ch < 0x25 || (0x2a < ch)))) &&
            ((((((ch < 0x2c || (0x2f < ch)) && (ch != 0x3a)) && ((ch != 0x3b && (ch != 0x3f)))) &&
              (ch != 0x40)) &&
             (((ch < 0x5b || (0x5d < ch)) &&
              ((ch != 0x5f && (((ch != 0x7b && (ch != 0x7d)) && (ch != 0xa1)))))))))) &&
           (((ch != 0xab && (ch != 0xad)) && ((ch != 0xb7 && ((ch != 0xbb && (ch != 0xbf)))))))) &&
          (ch != 0x37e)) && ((ch != 0x387 && (((ch < 0x55a || (0x55f < ch)) && (ch != 0x589)))))) &&
        (((ch != 0x58a && (ch != 0x5be)) &&
         ((ch != 0x5c0 &&
          ((((ch != 0x5c3 && (ch != 0x5c6)) &&
            ((ch != 0x5f3 &&
             ((((ch != 0x5f4 && (ch != 0x60c)) && (ch != 0x60d)) && ((ch != 0x61b && (ch != 0x61e)))
              ))))) && (ch != 0x61f)))))))) &&
       ((((((((ch < 0x66a || (0x66d < ch)) && ((ch != 0x6d4 && ((ch < 0x700 || (0x70d < ch)))))) &&
            ((ch < 0x7f7 || (0x7f9 < ch)))) &&
           ((((((ch != 0x964 && (ch != 0x965)) && (ch != 0x970)) &&
              ((ch != 0xdf4 && ((ch < 0xe4f || (0xe5b < ch)))))) && ((ch < 0xf04 || (0xf12 < ch))))
            && (((ch < 0xf3a || (0xf3d < ch)) && ((ch != 0xf85 && ((ch != 0xfd0 && (ch != 0xfd1)))))
                ))))) && ((ch < 0x104a || (0x104f < ch)))) &&
         (((((ch != 0x10fb && (((ch < 0x1361 || (0x1368 < ch)) && (ch != 0x166d)))) &&
            ((ch != 0x166e && (ch != 0x169b)))) && (ch != 0x169c)) &&
          (((((((ch < 0x16eb || (0x16ed < ch)) && ((ch != 0x1735 && (ch != 0x1736)))) &&
              ((ch < 0x17d4 || (0x17d6 < ch)))) &&
             (((ch < 0x17d8 || (0x17da < ch)) &&
              (((((ch < 0x1800 || (0x180a < ch)) && (ch != 0x1944)) &&
                ((ch != 0x1945 && (ch != 0x19de)))) &&
               ((ch != 0x19df && ((ch != 0x1a1e && (ch != 0x1a1f)))))))))) &&
            ((ch < 0x1b5a || (0x1b60 < ch)))) &&
           (((((ch < 0x2010 || (0x2027 < ch)) && ((ch < 0x2030 || (0x2043 < ch)))) &&
             ((ch < 0x2045 || (0x2051 < ch)))) && ((ch < 0x2053 || (0x205e < ch)))))))))) &&
        ((((ch != 0x207d && (ch != 0x207e)) && (ch != 0x208d)) &&
         (((ch != 0x208e && (ch != 0x2329)) && ((ch != 0x232a && ((ch < 0x2768 || (0x2775 < ch))))))
         )))))) && ((ch < 0x27c5 || (0x27c6 < ch)))) &&
     (((((((ch < 0x27e6 || (0x27eb < ch)) && ((ch < 0x2983 || (0x2998 < ch)))) &&
         (((((((ch < 0x29d8 || (0x29db < ch)) && (ch != 0x29fc)) && (ch != 0x29fd)) &&
            (((ch < 0x2cf9 || (0x2cfc < ch)) && ((ch != 0x2cfe && (ch != 0x2cff)))))) &&
           ((ch < 0x2e00 || (0x2e17 < ch)))) &&
          ((ch != 0x2e1c && ((ch != 0x2e1d && ((ch < 0x3001 || (0x3003 < ch)))))))))) &&
        ((ch < 0x3008 || (0x3011 < ch)))) &&
       ((((((((ch < 0x3014 || (0x301f < ch)) && (ch != 0x3030)) &&
            ((ch != 0x303d && (ch != 0x30a0)))) && (ch != 0x30fb)) &&
          (((ch < 0xa874 || (0xa877 < ch)) && ((ch != 0xfd3e && (ch != 0xfd3f)))))) &&
         ((ch < 0xfe10 || (0xfe19 < ch)))) && ((ch < 0xfe30 || (0xfe52 < ch)))))) &&
      (((((ch < 0xfe54 || (0xfe61 < ch)) && (ch != 0xfe63)) &&
        (((ch != 0xfe68 && (ch != 0xfe6a)) && ((ch != 0xfe6b && ((ch < 0xff01 || (0xff03 < ch)))))))
        ) && (((ch < 0xff05 || (0xff0a < ch)) &&
              (((((ch < 0xff0c || (0xff0f < ch)) && (ch != 0xff1a)) &&
                (((ch != 0xff1b && (ch != 0xff1f)) &&
                 (((ch != 0xff20 && ((ch < 0xff3b || (0xff3d < ch)))) && (ch != 0xff3f)))))) &&
               (((ch != 0xff5b && (ch != 0xff5d)) && ((ch < 0xff5f || (0xff65 < ch))))))))))))))) {
    return true;
    }
        return false;
}

// ---- <global> 自由函数 ----

CharString left(const CharString& str, int length)
{
    if (str.length() <= length)
    {
        return CharString(str);
    }
    else
    {
        return CharString(str.c_str(), length);
    }
}

CharString mid(const CharString& str, int start)
{
    if (start == 0)
    {
        return CharString(str);
    }
    else if (str.length() <= start)
    {
        return CharString();
    }
    else
    {
        return CharString(str.c_str() + start, str.length() - start);
    }
}

CharString mid(const CharString& str, int start, int length)
{
    if (str.length() <= start)
    {
        return CharString();
    }
    else
    {
        if ((start + length) > str.length())
        {
            length += str.length() - (start + length);
        }
        return CharString(str.c_str() + start, length);
    }
}

CharString right(const CharString& str, int length)
{
    // [b28c-16 定论 N-86b] 残差仅 `str.length() - length` 的物化段数：ORIG 4.1
    // 发 `mov %eax,%edx; mov 0x10(%ebp),%eax; mov %edx,%ecx; sub %eax,%ecx;
    // mov %ecx,%eax` 五段（无槽往返 ⇒ P0 无具名局部），4.4 恒折 `sub MEM,%eax`
    // 单条；同一算术多 codegen，源形不可择。
    // [b28d-34] 复核维持 N-86b（ORIG 0x08ad7fe5）。
    if (str.length() <= length)
    {
        return CharString(str);
    }
    else
    {
        return CharString(str.c_str() + (str.length() - length), length);
    }
}

WideString left(const WideString& str, int length)
{
    if (str.length() <= length)
    {
        return WideString(str);
    }
    else
    {
        return WideString(str.c_str(), length);
    }
}

WideString mid(const WideString& str, int start)
{
    if (start == 0)
    {
        return WideString(str);
    }
    else if (str.length() <= start)
    {
        return WideString();
    }
    else
    {
        return WideString(str.c_str() + start, str.length() - start);
    }
}

WideString mid(const WideString& str, int start, int length)
{
    if (str.length() <= start)
    {
        return WideString();
    }
    else
    {
        if ((start + length) > str.length())
        {
            length += str.length() - (start + length);
        }
        return WideString(str.c_str() + start, length);
    }
}

WideString right(const WideString& str, int length)
{
    // [b28c-16 定论 N-86b] 同 right(CharString)：4.1 五段物化 vs 4.4 单条折叠。
    // [b28d-34] 复核维持 N-86b（ORIG 0x08adb099）。
    if (str.length() <= length)
    {
        return WideString(str);
    }
    else
    {
        return WideString(str.c_str() + (str.length() - length), length);
    }
}

int ss_vsprintf(char* buffer, size_t count, const char* format, char* argList)
{
    // ORIG _Z11ss_vsprintfPcjPKcS_ @0x08ad457c
    int ret = vsnprintf(buffer, count, format, argList);
    if (static_cast<int>(count) <= ret)
    {
        *buffer = '\0';
        return 0;
    }
    return ret;
}

int ss_vswprintf(wchar_t* buffer, size_t count, const wchar_t* format, char* argList)
{
    // ORIG _Z12ss_vswprintfPwjPKwPc @0x08ad45f3
    int ret = vswprintf(buffer, count, format, argList);
    if (ret < 0 || static_cast<int>(count) <= ret)
    {
        *buffer = L'\0';
        return 0;
    }
    return ret;
}

int ss_strcpy(char* dest, size_t count, const char* src)
{
    // ORIG _Z9ss_strcpyPcjPKc @0x08ad463f：count 容纳 len+1 时 memcpy 并返回 0；
    // 否则 dest[0]=0 返回 1。
    int len = static_cast<int>(strlen(src));
    if (static_cast<int>(count) <= len)
    {
        *dest = '\0';
        return 1;
    }
    memcpy(dest, src, len + 1);
    return 0;
}

int ss_strcat(char* dest, size_t count, const char* src)
{
    // ORIG _Z9ss_strcatPcjPKc @0x08ad46dd：容纳 dlen+slen+1 时 memcpy 并返回 0；
    // 否则直接返回 1（不写字节）。
    int dlen = static_cast<int>(strlen(dest));
    int slen = static_cast<int>(strlen(src));
    if (static_cast<int>(count) <= dlen + slen)
    {
        return 1;
    }
    memcpy(dest + dlen, src, slen + 1);
    return 0;
}

// ---- CharString / WideString 自由运算符（ORIG 0x8ad959a..0x8ad997c /
// 0x08adc690..0x08adca62，委托 concat / compare；== 带长度/首字符预检）----

bool operator==(const CharString& src1, const char* src2)
{
    if (src1[0] != src2[0])
        return false;
    return CharString::compare(src1, src2) == 0;
}

bool operator==(char const* src1, const CharString& src2)
{
    if (src1[0] != src2[0])
        return false;
    return CharString::compare(src1, src2) == 0;
}

bool operator==(const CharString& src1, const CharString& src2)
{
    if (src1.length() != src2.length())
        return false;
    if (src1[0] != src2[0])
        return false;
    return CharString::compare(src1, src2) == 0;
}

bool operator!=(const CharString& src1, const char* src2)
{
    return CharString::compare(src1, src2) != 0;
}

bool operator!=(char const* src1, const CharString& src2)
{
    return CharString::compare(src1, src2) != 0;
}

bool operator!=(const CharString& src1, const CharString& src2)
{
    return CharString::compare(src1, src2) != 0;
}

bool operator<(const CharString& src1, const char* src2)
{
    return CharString::compare(src1, src2) < 0;
}

bool operator<(char const* src1, const CharString& src2)
{
    return CharString::compare(src1, src2) < 0;
}

bool operator<(const CharString& src1, const CharString& src2)
{
    return CharString::compare(src1, src2) < 0;
}

bool operator<=(const CharString& src1, const char* src2)
{
    return CharString::compare(src1, src2) <= 0;
}

bool operator<=(char const* src1, const CharString& src2)
{
    return CharString::compare(src1, src2) <= 0;
}

bool operator<=(const CharString& src1, const CharString& src2)
{
    return CharString::compare(src1, src2) <= 0;
}

bool operator>(const CharString& src1, const char* src2)
{
    return CharString::compare(src1, src2) > 0;
}

bool operator>(char const* src1, const CharString& src2)
{
    return CharString::compare(src1, src2) > 0;
}

bool operator>(const CharString& src1, const CharString& src2)
{
    return CharString::compare(src1, src2) > 0;
}

bool operator>=(const CharString& src1, const char* src2)
{
    return CharString::compare(src1, src2) >= 0;
}

bool operator>=(char const* src1, const CharString& src2)
{
    return CharString::compare(src1, src2) >= 0;
}

bool operator>=(const CharString& src1, const CharString& src2)
{
    return CharString::compare(src1, src2) >= 0;
}

CharString operator+(const CharString& src1, const char* src2)
{
    return CharString::concat(src1, src2);
}

CharString operator+(const CharString& src1, const CharString& src2)
{
    return CharString::concat(src1, src2);
}

CharString operator+(const CharString& src1, char ch)
{
    return CharString::concat(src1, ch);
}

CharString operator+(const char* src1, const CharString& src2)
{
    return CharString::concat(src1, src2);
}

CharString operator+(char ch, const CharString& src2)
{
    return CharString::concat(ch, src2);
}

bool operator==(const WideString& src1, const wchar_t* src2)
{
    if (src1[0] != src2[0])
        return false;
    return WideString::compare(src1, src2) == 0;
}

bool operator==(wchar_t const* src1, const WideString& src2)
{
    if (src1[0] != src2[0])
        return false;
    return WideString::compare(src1, src2) == 0;
}

bool operator==(const WideString& src1, const WideString& src2)
{
    if (src1.length() != src2.length())
        return false;
    if (src1[0] != src2[0])
        return false;
    return WideString::compare(src1, src2) == 0;
}

bool operator!=(const WideString& src1, const wchar_t* src2)
{
    return WideString::compare(src1, src2) != 0;
}

bool operator!=(wchar_t const* src1, const WideString& src2)
{
    return WideString::compare(src1, src2) != 0;
}

bool operator!=(const WideString& src1, const WideString& src2)
{
    return WideString::compare(src1, src2) != 0;
}

bool operator<(const WideString& src1, const wchar_t* src2)
{
    return WideString::compare(src1, src2) < 0;
}

bool operator<(wchar_t const* src1, const WideString& src2)
{
    return WideString::compare(src1, src2) < 0;
}

bool operator<(const WideString& src1, const WideString& src2)
{
    return WideString::compare(src1, src2) < 0;
}

bool operator<=(const WideString& src1, const wchar_t* src2)
{
    return WideString::compare(src1, src2) <= 0;
}

bool operator<=(wchar_t const* src1, const WideString& src2)
{
    return WideString::compare(src1, src2) <= 0;
}

bool operator<=(const WideString& src1, const WideString& src2)
{
    return WideString::compare(src1, src2) <= 0;
}

bool operator>(const WideString& src1, const wchar_t* src2)
{
    return WideString::compare(src1, src2) > 0;
}

bool operator>(wchar_t const* src1, const WideString& src2)
{
    return WideString::compare(src1, src2) > 0;
}

bool operator>(const WideString& src1, const WideString& src2)
{
    return WideString::compare(src1, src2) > 0;
}

bool operator>=(const WideString& src1, const wchar_t* src2)
{
    return WideString::compare(src1, src2) >= 0;
}

bool operator>=(wchar_t const* src1, const WideString& src2)
{
    return WideString::compare(src1, src2) >= 0;
}

bool operator>=(const WideString& src1, const WideString& src2)
{
    return WideString::compare(src1, src2) >= 0;
}

WideString operator+(const WideString& src1, const wchar_t* src2)
{
    return WideString::concat(src1, src2);
}

WideString operator+(const WideString& src1, const WideString& src2)
{
    return WideString::concat(src1, src2);
}

WideString operator+(const WideString& src1, wchar_t ch)
{
    return WideString::concat(src1, ch);
}

WideString operator+(const wchar_t* src1, const WideString& src2)
{
    return WideString::concat(src1, src2);
}

WideString operator+(wchar_t ch, const WideString& src2)
{
    return WideString::concat(ch, src2);
}

// ---- StringUnorderedMap 显式实例化（CNRDScriptData 等使用）----
// [b28d-34] StringUnorderedMap<...,float>/<...,int>::insert（ORIG 0x08080b6c）：
// 复核维持 DIFF——clean 96=96，残差仅末尾析构 call 落点（ORIG 在 jmp 之后、
// 我们在 jmp 之前），N104/EH 尾块序封顶，语义等价。
#include "HashMap.h"

template class StringUnorderedMap<CharStringHashTrait, std::string>;
template class StringUnorderedMap<CharStringHashTrait, float>;
template class StringUnorderedMap<CharStringHashTrait, int>;
