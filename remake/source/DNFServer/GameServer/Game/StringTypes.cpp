// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringTypes/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "StringTypes/incRef.inc"
{
    // ORIG: lock addl $0x1,(%eax)
    __sync_fetch_and_add(&m_refCount, 1);
}

#include "StringTypes/decRef.inc"
{
    // ORIG: lock xadd -1；新值 <=0 则 free(this)
    if (__sync_sub_and_fetch(&m_refCount, 1) <= 0)
        ::free(this);
}

#include "StringTypes/isUnique.inc"
{
    return m_refCount == 1;
}

#include "StringTypes/isShared.inc"
{
    return m_refCount > 1;
}

#include "StringTypes/getSize.inc"
{
    return m_size;
}

#include "StringTypes/getBuffer.inc"
{
    return reinterpret_cast<unsigned char*>(this + 1);
}

#include "StringTypes/create.inc"
{
    StringData* data = static_cast<StringData*>(malloc(size + 8));
    if (data == 0)
        return 0;
    data->m_refCount = 0;
    data->m_size = size;
    return data;
}

// ---- CharStringData ----

#include "StringTypes/getLength.inc"
{
    return m_size - 1;
}

#include "StringTypes/getBuffer_1.inc"
{
    return reinterpret_cast<char*>(this + 1);
}

#include "StringTypes/create_1.inc"
{
    int size = length + 1;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<CharStringData*>(data);
}

#include "StringTypes/createTerminated.inc"
{
    CharStringData* data = create(length);
    data->getBuffer()[length] = '\0';
    return data;
}

// ---- WideStringData ----

#include "StringTypes/getLength_1.inc"
{
    return static_cast<int>(static_cast<unsigned int>(m_size) >> 2) - 1;
}

#include "StringTypes/getBuffer_2.inc"
{
    return reinterpret_cast<wchar_t*>(this + 1);
}

#include "StringTypes/create_2.inc"
{
    int size = (length + 1) * 4;
    StringData* data = StringData::create(size);
    if (data == 0)
        return 0;
    return static_cast<WideStringData*>(data);
}

#include "StringTypes/createTerminated_1.inc"
{
    WideStringData* data = create(length);
    data->getBuffer()[length] = 0;
    return data;
}

// ---- CharString 生命周期与数据访问 ----

#include "StringTypes/CharString.inc"
{
    attachData(s_emptyCharStringData);
}

#include "StringTypes/CharString_1.inc"
{
    CharStringData* newData = src.getData();
    attachData(newData);
}

#include "StringTypes/CharString_2.inc"
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

#include "StringTypes/CharString_3.inc"
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

#include "StringTypes/CharString_4.inc"
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

#include "StringTypes/CharString_5.inc"
{
    CharStringData* data = CharStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

#include "StringTypes/CharString_6.inc"
{
    attachData(newData);
}

#include "StringTypes/dtor_CharString.inc"
{
    getData()->decRef();
}

#include "StringTypes/c_str.inc"
{
    return buffer_;
}

#include "StringTypes/length.inc"
{
    return getData()->getLength();
}

#include "StringTypes/isEmpty.inc"
{
    return length() == 0;
}

#include "StringTypes/isWhiteSpace.inc"
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

#include "StringTypes/getHash.inc"
{
    // [L4e 2026-09-11 · 负结论：38/39，残余＝回边 jmp 后 1 条 `nop`] ORIG 0x08ad6c7c
    // `cmpl $0,-0x14(%ebp)` 直接紧随 0x08ad6c7a `jmp`（无填充）。根因＝后端版本行为：
    // 同形最小复现 /tmp/exp/t1.cpp(-m32) 在 cc1plus 4.4.4/4.4.6/4.4.7 均生成该 nop，
    // c5root GCC 4.1.2 同源输出无 nop 且逐条全等；8 个候选旗标（-fno-exceptions/
    // -falign-loops=1/-fno-reorder-blocks/…）无一消除 ⇒ 源码层不可修；整 TU 换 4.1.2
    // 亦不可行（HashMap.h 等 4.4-only 语法编译失败）。
    // [b28d-solo-StringTypes] verdict=AE：残差仅 N112 循环出口对齐 nop（§6.1 豁免）。
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

#include "StringTypes/getAt.inc"
{
    return buffer_[idx];
}

#include "StringTypes/setAt.inc"
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6ca2）：①删 `char* dest` 具名缓存
    // （P183），memcpy 三实参按 ORIG 求值序内联——size(`getLength()+1`)→edi、
    // src(`thisData->getBuffer()`)→esi、dst(`newData->getBuffer()`)→eax；
    // ②两个指针局部按 P212 全头部声明（thisData 先 ⇒ 4.4 先声明深 -0x20、
    // newData 后 ⇒ -0x1c），复现 ORIG 槽序（旧 N82 判词作废）。
    CharStringData* thisData;
    CharStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        thisData = getData();
        newData = CharStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(), thisData->getLength() + 1);
        newData->getBuffer()[idx] = ch;
        return CharString(newData);
    }
}

#include "StringTypes/startsWith.inc"
{
    int patLength = static_cast<int>(strlen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str(), pat, patLength) == 0;
    else
        return strncmp(c_str(), pat, patLength) == 0;
}

#include "StringTypes/endsWith.inc"
{
    int patLength = static_cast<int>(strlen(pat));
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6e02）：早退 `if (length() < patLength)
    // return false;` 保持；残差仅 `length() - patLength` 物化段（同 right 族），
    // 改左结合 `c_str() + length() - patLength` 复现 ORIG 六段
    // （P205 拆形族；旧 N-86b 判词作废）。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return strncasecmp(c_str() + length() - patLength, pat, patLength) == 0;
    return strncmp(c_str() + length() - patLength, pat, patLength) == 0;
}

#include "StringTypes/front.inc"
{
    return *buffer_;
}

#include "StringTypes/back.inc"
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad6edc）：`if(length())` 形在 4.4
    // 恒物化 `test %eax,%eax; setne %al; test %al,%al; je`（P292 int callee 侧），
    // ORIG 4.1 为直判 `test %eax,%eax; je`。改三目 `cond ? a : b`（P47 族）
    // 即复现直判 + 零分支后置岛布局，逐条与 ORIG 全等。
    return length() ? buffer_[length() - 1] : '\0';
}

#include "StringTypes/begin.inc"
{
    return buffer_;
}

#include "StringTypes/end.inc"
{
    return buffer_ + length();
}

#include "StringTypes/assign.inc"
{
    // ORIG 0x08ad6fed：`cmp %eax,%ebx; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223）。while+break 空体被 4.4 折掉 jne（实测 -3 条）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
}

#include "StringTypes/assign_1.inc"
{
    assign(src, static_cast<int>(strlen(src)));
}

#include "StringTypes/assign_2.inc"
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

#include "StringTypes/assign_3.inc"
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

#include "StringTypes/clear.inc"
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

#include "StringTypes/attachData.inc"
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

#include "StringTypes/replaceData.inc"
{
    CharStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

#include "StringTypes/getData.inc"
{
    return reinterpret_cast<CharStringData*>(buffer_ - 8);
}

// ---- CharString::find / rfind ----

#include "StringTypes/find.inc"
{
    return find(0, pat, static_cast<int>(strlen(pat)));
}

#include "StringTypes/find_1.inc"
{
    return find(pos, pat, static_cast<int>(strlen(pat)));
}

#include "StringTypes/find_2.inc"
{
    return find(0, pat, patLength);
}

#include "StringTypes/find_3.inc"
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

#include "StringTypes/find_4.inc"
{
    return find(0, pat.c_str(), pat.length());
}

#include "StringTypes/find_5.inc"
{
    return find(pos, pat.c_str(), pat.length());
}

#include "StringTypes/find_6.inc"
{
    return find(0, pat);
}

#include "StringTypes/find_7.inc"
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

#include "StringTypes/rfind.inc"
{
    int patLength = static_cast<int>(strlen(pat));
    return rfind(length() - patLength, pat, patLength);
}

#include "StringTypes/rfind_1.inc"
{
    return rfind(pos, pat, static_cast<int>(strlen(pat)));
}

#include "StringTypes/rfind_2.inc"
{
    return rfind(length() - patLength, pat, patLength);
}

#include "StringTypes/rfind_3.inc"
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

#include "StringTypes/rfind_4.inc"
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

#include "StringTypes/rfind_5.inc"
{
    return rfind(pos, pat.c_str(), pat.length());
}

#include "StringTypes/rfind_6.inc"
{
    return rfind(length() - 1, pat);
}

#include "StringTypes/rfind_7.inc"
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

#include "StringTypes/concat.inc"
{
    return concat(src1, static_cast<int>(strlen(src1)),
                  src2, static_cast<int>(strlen(src2)));
}

#include "StringTypes/concat_1.inc"
                              const char* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len);
    memcpy(newData->getBuffer() + src1Len, src2, src2Len);
    return CharString(newData);
}

#include "StringTypes/concat_2.inc"
{
    return concat(src1, src2, static_cast<int>(strlen(src2)));
}

#include "StringTypes/concat_3.inc"
{
    if (src1.length() == 0)
        return CharString(src2, src2Len);
    if (src2Len == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
}

#include "StringTypes/concat_4.inc"
{
    return concat(src1, static_cast<int>(strlen(src1)), src2);
}

#include "StringTypes/concat_5.inc"
{
    if (src1Len == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
}

#include "StringTypes/concat_6.inc"
{
    if (src1.length() == 0)
        return CharString(src2);
    if (src2.length() == 0)
        return CharString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
}

#include "StringTypes/concat_7.inc"
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

#include "StringTypes/concat_8.inc"
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

#include "StringTypes/pattern.inc"
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

#include "StringTypes/format.inc"
{
    // b28d-Main：改为 static sret 按值（ORIG 0x08ad8afa 尾 ret $0x4、fmt@0xc/varargs@0x10）。
    va_list argList;
    va_start(argList, formatStr);
    // [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08ad8afa）：ORIG format 体是 vformat 逻辑的
    // **独立副本**（体内无 call vformat；4.1 前端把头内 inline vformat 展开进 format）。
    // 旧「委托 vformat」形只 15 条 vs ORIG 53 条且调用点差集巨大，改为展开完整逻辑；
    // 展开后 53 vs 62，余下 9 条全为 N83（placement operator new 4.4 恒发
    // _ZnwjPv call+判空，两处 ×4~5）⇒ 封顶，语义等价。
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        va_end(argList);
        return CharString();
    }
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    va_end(argList);
    return CharString(newData);
}

// ORIG T 0x08ad8b9e：length==0 → this->C1Ev()（**就地默认构造覆盖**，旧 data 不 detach——
// ORIG 体实证无 ~CharString/D1 调用）；否则 create(length)+ss_vsprintf 后 this->C1EP14CharStringData
// 就地覆盖；尾 return *this。
// [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08ad8b9e）：ORIG 两处就地覆盖为
// 单条 `call CharString::C1Ev` / `call CharString::C1EP14CharStringData`（4.1 前端
// 内联 placement new）；4.4 对 `new (this) T(...)` 恒发 `_ZnwjPv` call + 判空
// （每处 +4~5 条，58 vs 49）。显式构造调用语法 `this->CharString::CharString()`
// 在 4.4 编译报「错误地使用了 struct」（探针 /tmp/s23/p3.cpp）⇒ 无合法源形，
// 语义等价（就地构造覆盖、旧 data 不 detach 均保持）。
#include "StringTypes/vformat.inc"
{
    // b28d-Main：static sret 按值（ORIG 0x08ad8b9e）。
    int length = vsnprintf(reinterpret_cast<char*>(0), 0, formatStr, argList);
    if (length == 0)
        return CharString();
    CharStringData* newData = CharStringData::create(length);
    ss_vsprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return CharString(newData);
}

#include "StringTypes/tokenize.inc"
{
    return tokenize(str, tokenVector, " \t\n\r", ".,", false);
}

#include "StringTypes/tokenize_1.inc"
                         const char* dropDelimiters, const char* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-solo-StringTypes] 收敛（ORIG 0x08ad8c6c）：①ORIG 的 `*d0` 判定为
    // `test *d0; je <远处 d1 分支>` + drop 分支直落；4.4 对原 if/else 顺序产
    // 反向布局（jne 跳 else + d1 分支直落），交换两臂并取反条件即复现块序。
    // ②按 P212 全头部声明（i/c/d0/d1/drop-newLen，声明序=ORIG 槽深序）复现
    // ORIG 槽位 i=-0x20/c=-0x19/d0=-0x18/d1=-0x14/newLen=-0x10。
    const char* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    int i;
    char c;
    const char* d0;
    const char* d1;
    int newLen;
    for (i = 0; i < strLen; ++i)
    {
        c = strBuf[i];
        d0 = dropDelimiters;
        while (*d0 != '\0' && *d0 != c)
            ++d0;
        if (*d0 != '\0')
        {
            newLen = i - nextStartIdx;
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
        else
        {
            d1 = returnDelimiters;
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
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

#include "StringTypes/tokenizeNewLine.inc"
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

#include "StringTypes/join.inc"
                            const CharString& separator)
{
    // [R22 2026-09-10] DIFF(gap=-30) → NEAR 196=196（判据 P292/P168a + 探针
    // /tmp/p_r22{,b,c,d}.cpp 定形）：ORIG 0x08ad9268/0x08ad9348 两遍扫描各带一个
    // runOnce 守卫——`movb $0,-0x19` → 头部 `cmpb $0,-0x19; jne <跳过整个循环块>`
    // → `&&` 右侧 `xorb $1,-0x19; cmpb $0,-0x19; je` → 体内 `while(runOnce){…;=false;}`。
    // 4.4 复现三要素：① 变量必须是 **bool**（uchar 时 while 恒 setne 物化，bool 才
    // `cmpb;jne` 直跳）；② 头部守卫写「`if (x) {} else { 整个循环块 }`」（反置空 then
    // ⇒ `cmpb;jne` 直跳；写 `if (!x)` 触发 4.4 bool 取反 xor 物化 N122）；③ 条件写
    // `(x = !x)`（产 ORIG 的 xorb 就地异或）。原残余的槽位分配序差（-0x21/-0x15 vs
    // ORIG -0x19/-0x11）已由下方「槽序归零组合旋钮」归零（compare_tu 口径 near→ae）。
    if (stringVector.empty())
    {
        return CharString();
    }
    const char* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    // [L4e 2026-09-11 · NEAR→AE：槽序归零组合旋钮] ① newData/newDataBuf 顶层**声明**
    // 提前到首个 runOnce 之前（赋值留原位）；② 两个 runOnce bool 移入各自块内。
    // 单做①会整体平移 4B（更差），必须与②同做 ⇒ GCC4.4 槽序＝ORIG：newData -0x24 /
    // newDataBuf -0x20 / bool1 -0x19(槽 -0x1c) / token1 -0x18 / bool2 -0x11(槽 -0x14)。
    CharStringData* newData;
    char* newDataBuf;
    {
        // [R22] ORIG 0x08ad9268/0x08ad9348 两遍扫描各带一个 runOnce bool 守卫
        // （movb $0,-0x19 / -0x11 → 循环头 cmpb+jne 跳过 → 条件段 xorb $1 取反），
        // 内层 while(runOnce){...; runOnce=false;} 只跑一次。照抄该形状。
        bool token_runOnce = false;
        if (token_runOnce)
        {
        }
        else
        {
            std::vector<CharString>::const_iterator it = stringVector.begin();
            std::vector<CharString>::const_iterator itEnd = stringVector.end();
            while (it != itEnd && (token_runOnce = !token_runOnce))
            {
                const CharString& token = *it;
                while (token_runOnce)
                {
                    totalLength += token.length();
                    token_runOnce = false;
                }
                it++;
            }
        }
    }
    if (totalLength == 0)
    {
        return CharString();
    }
    newData = CharStringData::createTerminated(totalLength);
    newDataBuf = newData->getBuffer();
    {
        bool token_runOnce2 = false;
        if (token_runOnce2)
        {
        }
        else
        {
            std::vector<CharString>::const_iterator it2 = stringVector.begin();
            std::vector<CharString>::const_iterator itEnd2 = stringVector.end();
            while (it2 != itEnd2 && (token_runOnce2 = !token_runOnce2))
            {
                const CharString& token = *it2;
                while (token_runOnce2)
                {
                    if (separatorStrLen >= 1 && it2 != stringVector.begin())
                    {
                        memcpy(newDataBuf, separatorStrSrc, separatorStrLen);
                        newDataBuf += separatorStrLen;
                    }
                    int tokenLength = token.length();
                    memcpy(newDataBuf, token.c_str(), tokenLength);
                    newDataBuf += tokenLength;
                    token_runOnce2 = false;
                }
                it2++;
            }
        }
    }
    return CharString(newData);
}

#include "StringTypes/compare.inc"
{
    return strcmp(src1.c_str(), src2);
}

#include "StringTypes/compare_1.inc"
{
    return strcmp(src1.c_str(), src2.c_str());
}

#include "StringTypes/compare_2.inc"
{
    return strcmp(src1, src2.c_str());
}

#include "StringTypes/compareNoCase.inc"
{
    return strcasecmp(src1.c_str(), src2);
}

#include "StringTypes/compareNoCase_1.inc"
{
    return strcasecmp(src1.c_str(), src2.c_str());
}

#include "StringTypes/compareNoCase_2.inc"
{
    return strcasecmp(src1, src2.c_str());
}

// ---- WideString 生命周期与数据访问 ----

#include "StringTypes/WideString.inc"
{
    attachData(s_emptyWideStringData);
}

#include "StringTypes/WideString_1.inc"
{
    WideStringData* newData = src.getData();
    attachData(newData);
}

#include "StringTypes/WideString_2.inc"
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

#include "StringTypes/WideString_3.inc"
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

#include "StringTypes/WideString_4.inc"
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

#include "StringTypes/WideString_5.inc"
{
    WideStringData* data = WideStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

#include "StringTypes/WideString_6.inc"
{
    attachData(newData);
}

#include "StringTypes/dtor_WideString.inc"
{
    getData()->decRef();
}

#include "StringTypes/c_str_1.inc"
{
    return buffer_;
}

#include "StringTypes/length_1.inc"
{
    return getData()->getLength();
}

#include "StringTypes/isEmpty_1.inc"
{
    return length() == 0;
}

#include "StringTypes/isWhiteSpace_1.inc"
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

#include "StringTypes/getHash_1.inc"
{
    // [L4e 2026-09-11 · 负结论：38/39，残余＝回边 jmp 后 1 条 `nop`] ORIG 0x08ad9cd2
    // `cmpl $0,-0x14(%ebp)` 直接紧随 0x08ad9cd0 `jmp`（无填充）；根因同 CharString::getHash
    // （4.4.x 循环尾 nop / GCC 4.1.2 无，8 旗标无效）⇒ 源码层不可修。
    // [b28d-solo-StringTypes] verdict=AE：残差仅 N112 循环出口对齐 nop（§6.1 豁免）。
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

#include "StringTypes/getAt_1.inc"
{
    return buffer_[idx];
}

#include "StringTypes/setAt_1.inc"
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9cf8）：同 CharString::setAt，
    // 删 `wchar_t* dest` 缓存 + P212 全头部声明复现槽序。
    WideStringData* thisData;
    WideStringData* newData;
    if (buffer_[idx] == ch)
    {
        return *this;
    }
    else
    {
        thisData = getData();
        newData = WideStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(),
               (thisData->getLength() + 1) * sizeof(wchar_t));
        newData->getBuffer()[idx] = ch;
        return WideString(newData);
    }
}

#include "StringTypes/startsWith_1.inc"
{
    int patLength = static_cast<int>(wcslen(pat));
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str(), pat, patLength) == 0;
    else
        return wcsncmp(c_str(), pat, patLength) == 0;
}

#include "StringTypes/endsWith_1.inc"
{
    int patLength = static_cast<int>(wcslen(pat));
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9df8）：同 CharString::endsWith，
    // 左结合 `c_str() + length() - patLength` 复现 4.1 六段物化（P205 拆形族）。
    if (length() < patLength)
        return false;
    if (isIgnoreCase)
        return wcsncasecmp(c_str() + length() - patLength, pat, patLength) == 0;
    return wcsncmp(c_str() + length() - patLength, pat, patLength) == 0;
}

#include "StringTypes/front_1.inc"
{
    return *buffer_;
}

#include "StringTypes/back_1.inc"
{
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad9f36）：同 CharString::back，
    // 三目复现 4.1 直判 `test;je` + 零值岛后置（P47 族）。
    return length() ? buffer_[length() - 1] : 0;
}

#include "StringTypes/begin_1.inc"
{
    return buffer_;
}

#include "StringTypes/end_1.inc"
{
    return buffer_ + length();
}

#include "StringTypes/assign_4.inc"
{
    // ORIG 0x08ad9feb：`cmp; sete %al; test %al,%al; jne +0x44(nop 岛)`
    // ⇒ goto 尾空标签形（P223，同 CharString::assign）。
    if (getData() == src.getData())
        goto done;
    replaceData(src.getData());
done:;
}

#include "StringTypes/assign_5.inc"
{
    assign(src, static_cast<int>(wcslen(src)));
}

#include "StringTypes/assign_6.inc"
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

#include "StringTypes/assign_7.inc"
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

#include "StringTypes/clear_1.inc"
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

#include "StringTypes/attachData_1.inc"
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

#include "StringTypes/replaceData_1.inc"
{
    WideStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

#include "StringTypes/getData_1.inc"
{
    return reinterpret_cast<WideStringData*>(reinterpret_cast<char*>(buffer_) - 8);
}

// ---- WideString::find / rfind ----

#include "StringTypes/find_8.inc"
{
    return find(0, pat, static_cast<int>(wcslen(pat)));
}

#include "StringTypes/find_9.inc"
{
    return find(pos, pat, static_cast<int>(wcslen(pat)));
}

#include "StringTypes/find_10.inc"
{
    return find(0, pat, patLength);
}

#include "StringTypes/find_11.inc"
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

#include "StringTypes/find_12.inc"
{
    return find(0, pat.c_str(), pat.length());
}

#include "StringTypes/find_13.inc"
{
    return find(pos, pat.c_str(), pat.length());
}

#include "StringTypes/find_14.inc"
{
    return find(0, pat);
}

#include "StringTypes/find_15.inc"
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

#include "StringTypes/rfind_8.inc"
{
    int patLength = static_cast<int>(wcslen(pat));
    return rfind(length() - patLength, pat, patLength);
}

#include "StringTypes/rfind_9.inc"
{
    return rfind(pos, pat, static_cast<int>(wcslen(pat)));
}

#include "StringTypes/rfind_10.inc"
{
    return rfind(length() - patLength, pat, patLength);
}

#include "StringTypes/rfind_11.inc"
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

#include "StringTypes/rfind_12.inc"
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

#include "StringTypes/rfind_13.inc"
{
    return rfind(pos, pat.c_str(), pat.length());
}

#include "StringTypes/rfind_14.inc"
{
    return rfind(length() - 1, pat);
}

#include "StringTypes/rfind_15.inc"
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

#include "StringTypes/concat_9.inc"
{
    return concat(src1, static_cast<int>(wcslen(src1)),
                  src2, static_cast<int>(wcslen(src2)));
}

#include "StringTypes/concat_10.inc"
                              const wchar_t* src2, int src2Len)
{
    if (src1Len + src2Len == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(src1Len + src2Len);
    memcpy(newData->getBuffer(), src1, src1Len * sizeof(wchar_t));
    memcpy(newData->getBuffer() + src1Len, src2, src2Len * sizeof(wchar_t));
    return WideString(newData);
}

#include "StringTypes/concat_11.inc"
{
    return concat(src1, src2, static_cast<int>(wcslen(src2)));
}

#include "StringTypes/concat_12.inc"
{
    if (src1.length() == 0)
        return WideString(src2, src2Len);
    if (src2Len == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2, src2Len);
}

#include "StringTypes/concat_13.inc"
{
    return concat(src1, static_cast<int>(wcslen(src1)), src2);
}

#include "StringTypes/concat_14.inc"
{
    if (src1Len == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1, src1Len);
    return concat(src1, src1Len, src2.c_str(), src2.length());
}

#include "StringTypes/concat_15.inc"
{
    if (src1.length() == 0)
        return WideString(src2);
    if (src2.length() == 0)
        return WideString(src1);
    return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
}

#include "StringTypes/concat_16.inc"
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

#include "StringTypes/concat_17.inc"
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

#include "StringTypes/pattern_1.inc"
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

#include "StringTypes/format_1.inc"
{
    // b28d-Main：static sret 按值（ORIG 0x08adbbe4）。
    va_list argList;
    va_start(argList, formatStr);
    // [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08adbbe4）：同 CharString::format——
    // ORIG 体是 vformat 逻辑的独立副本（无 call vformat），展开完整逻辑；
    // 余下仅 N83（placement operator new）封顶。
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
    {
        va_end(argList);
        return WideString();
    }
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    va_end(argList);
    return WideString(newData);
}

// CharString::vformat 同形（ORIG T 0x08adbc88）。
// [b28d-solo-StringTypes] verdict=N83 封顶（ORIG 0x08adbc88）：同 CharString::vformat
// （4.4 placement operator new 恒发 `_ZnwjPv` call + 判空，无合法源形可内联）。
#include "StringTypes/vformat_1.inc"
{
    // b28d-Main：static sret 按值（ORIG 0x08adbc88）。
    int length = vswprintf(reinterpret_cast<wchar_t*>(0), 0, formatStr, argList);
    if (length == 0)
        return WideString();
    WideStringData* newData = WideStringData::create(length);
    ss_vswprintf(newData->getBuffer(), length + 1, formatStr, argList);
    return WideString(newData);
}

#include "StringTypes/tokenize_2.inc"
{
    return tokenize(str, tokenVector, L" \t\n\r", L".", false);
}

#include "StringTypes/tokenize_3.inc"
                         const wchar_t* dropDelimiters, const wchar_t* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-solo-StringTypes] 收敛（ORIG 0x08adbd56）：同 CharString::tokenize，
    // 交换两臂 + 取反条件复现 ORIG 的 `je <远处 d1 分支>` + drop 分支直落块序，
    // 并按 P212 全头部声明复现槽位。
    const wchar_t* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    int i;
    wchar_t c;
    const wchar_t* d0;
    const wchar_t* d1;
    int newLen;
    for (i = 0; i < strLen; ++i)
    {
        c = strBuf[i];
        d0 = dropDelimiters;
        while (*d0 != 0 && *d0 != c)
            ++d0;
        if (*d0 != 0)
        {
            newLen = i - nextStartIdx;
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
        else
        {
            d1 = returnDelimiters;
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
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

#include "StringTypes/tokenizeNewLine_1.inc"
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

#include "StringTypes/join_1.inc"
                            const WideString& separator)
{
    // [R22 2026-09-10] DIFF(gap=-30) → NEAR 200=200：同 CharString::join 的 runOnce
    // 守卫复现（ORIG 0x08adc34e/0x08adc42e）。残余为 1 字节局部槽位分配序差
    // （P112/N82 族）已由下方「槽序归零组合旋钮」归零（compare_tu 口径 near→ae）。
    if (stringVector.empty())
    {
        return WideString();
    }
    const wchar_t* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    // [L4e 2026-09-11] 同 CharString::join：newData/newDataBuf 顶层声明提前 + 两个
    // runOnce bool 移入各自块 ⇒ 槽序复现 ORIG（newData -0x24 / newDataBuf -0x20 /
    // bool1 -0x19 / token1 -0x18）。
    WideStringData* newData;
    wchar_t* newDataBuf;
    {
        // [R22] 同 CharString::join（ORIG 0x08adc34e/0x08adc42e 两遍 runOnce 守卫）。
        bool token_runOnce = false;
        if (token_runOnce)
        {
        }
        else
        {
            std::vector<WideString>::const_iterator it = stringVector.begin();
            std::vector<WideString>::const_iterator itEnd = stringVector.end();
            while (it != itEnd && (token_runOnce = !token_runOnce))
            {
                const WideString& token = *it;
                while (token_runOnce)
                {
                    totalLength += token.length();
                    token_runOnce = false;
                }
                it++;
            }
        }
    }
    if (totalLength == 0)
    {
        return WideString();
    }
    newData = WideStringData::createTerminated(totalLength);
    newDataBuf = newData->getBuffer();
    {
        bool token_runOnce2 = false;
        if (token_runOnce2)
        {
        }
        else
        {
            std::vector<WideString>::const_iterator it2 = stringVector.begin();
            std::vector<WideString>::const_iterator itEnd2 = stringVector.end();
            while (it2 != itEnd2 && (token_runOnce2 = !token_runOnce2))
            {
                const WideString& token = *it2;
                while (token_runOnce2)
                {
                    if (separatorStrLen >= 1 && it2 != stringVector.begin())
                    {
                        memcpy(newDataBuf, separatorStrSrc, separatorStrLen * sizeof(wchar_t));
                        newDataBuf += separatorStrLen;
                    }
                    int tokenLength = token.length();
                    memcpy(newDataBuf, token.c_str(), tokenLength * sizeof(wchar_t));
                    newDataBuf += tokenLength;
                    token_runOnce2 = false;
                }
                it2++;
            }
        }
    }
    return WideString(newData);
}

#include "StringTypes/compare_3.inc"
{
    return wcscmp(src1.c_str(), src2);
}

#include "StringTypes/compare_4.inc"
{
    return wcscmp(src1.c_str(), src2.c_str());
}

#include "StringTypes/compare_5.inc"
{
    return wcscmp(src1, src2.c_str());
}

#include "StringTypes/compareNoCase_3.inc"
{
    return wcscasecmp(src1.c_str(), src2);
}

#include "StringTypes/compareNoCase_4.inc"
{
    return wcscasecmp(src1.c_str(), src2.c_str());
}

#include "StringTypes/compareNoCase_5.inc"
{
    return wcscasecmp(src1, src2.c_str());
}

#include "StringTypes/isuspace.inc"
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

#include "StringTypes/isupunct.inc"
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
    // [b28d-solo-StringTypes] 归零（ORIG 0x08ad7fe5）：旧 N-86b 判词作废。残差
    // `str.length() - length` 的物化段数由**表达式结合序**决定：写
    // `p + (len - length)` 时 4.4 折 `sub MEM,%eax` 单条；改左结合
    // `p + len - length`（指针先加长度再减，语义等价）即复现 ORIG 六段
    // `mov %eax,%edx; mov 0x10(%ebp),%eax; mov %edx,%ecx; sub %eax,%ecx;
    // mov %ecx,%eax`（P205 拆形族，探针 /tmp/s23/p2.cpp r3 逐条命中）。
    if (str.length() <= length)
    {
        return CharString(str);
    }
    else
    {
        return CharString(str.c_str() + str.length() - length, length);
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
    // [b28d-solo-StringTypes] 归零（ORIG 0x08adb099）：同 right(CharString)，
    // 左结合 `p + len - length` 复现 4.1 六段物化（P205 拆形族）。
    if (str.length() <= length)
    {
        return WideString(str);
    }
    else
    {
        return WideString(str.c_str() + str.length() - length, length);
    }
}

#include "StringTypes/ss_vsprintf.inc"
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

#include "StringTypes/ss_vswprintf.inc"
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

#include "StringTypes/ss_strcpy.inc"
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

#include "StringTypes/ss_strcat.inc"
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
// [b28d-solo-StringTypes] StringUnorderedMap<...,float>::insert verdict=N104 封顶
// （ORIG 0x08080b6c）：clean 96=96，助记符序列全等，唯一残差是 EH 尾块相对序——
// ORIG 把正常路径的 Value D1 放函数最末（0x8080c82），4.4 就地内联在 jmp 前、
// unwind 块压最末（N104：块内容逐条全等、两尾块互换）⇒ 语义等价、N 封顶。
// <...,int>::insert 同模板实例，同判。
#include "HashMap.h"

// [b28d-Main·检查点18] 原 3 条 StringUnorderedMap 显式实例化已删（同 StaticPool/
//   GlobalInstance 问题）：显式实例化发射**全部**成员，而 ORIG 只发射实际使用的
//   （end/get/begin/ValueEx::operator= 在 ORIG 中不存在 ⇒ 此前为 only-NEW）。
//   定义在 HashMap.h 类体内 ⇒ 按使用点隐式实例化即与 ORIG 一致。
