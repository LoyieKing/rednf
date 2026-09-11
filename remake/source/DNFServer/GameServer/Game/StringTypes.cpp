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


#include "StringTypes/decRef.inc"


#include "StringTypes/isUnique.inc"


#include "StringTypes/isShared.inc"


#include "StringTypes/getSize.inc"


#include "StringTypes/getBuffer.inc"


#include "StringTypes/create.inc"


// ---- CharStringData ----

#include "StringTypes/getLength.inc"


#include "StringTypes/getBuffer_1.inc"


#include "StringTypes/create_1.inc"


#include "StringTypes/createTerminated.inc"


// ---- WideStringData ----

#include "StringTypes/getLength_1.inc"


#include "StringTypes/getBuffer_2.inc"


#include "StringTypes/create_2.inc"


#include "StringTypes/createTerminated_1.inc"


// ---- CharString 生命周期与数据访问 ----

#include "StringTypes/CharString.inc"


#include "StringTypes/CharString_1.inc"


#include "StringTypes/CharString_2.inc"


#include "StringTypes/CharString_3.inc"


#include "StringTypes/CharString_4.inc"


#include "StringTypes/CharString_5.inc"


#include "StringTypes/CharString_6.inc"


#include "StringTypes/dtor_CharString.inc"


#include "StringTypes/c_str.inc"


#include "StringTypes/length.inc"


#include "StringTypes/isEmpty.inc"


#include "StringTypes/isWhiteSpace.inc"


#include "StringTypes/getHash.inc"


#include "StringTypes/getAt.inc"


#include "StringTypes/setAt.inc"


#include "StringTypes/startsWith.inc"


#include "StringTypes/endsWith.inc"


#include "StringTypes/front.inc"


#include "StringTypes/back.inc"


#include "StringTypes/begin.inc"


#include "StringTypes/end.inc"


#include "StringTypes/assign.inc"


#include "StringTypes/assign_1.inc"


#include "StringTypes/assign_2.inc"


#include "StringTypes/assign_3.inc"


#include "StringTypes/clear.inc"


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


#include "StringTypes/replaceData.inc"


#include "StringTypes/getData.inc"


// ---- CharString::find / rfind ----

#include "StringTypes/find.inc"


#include "StringTypes/find_1.inc"


#include "StringTypes/find_2.inc"


#include "StringTypes/find_3.inc"


#include "StringTypes/find_4.inc"


#include "StringTypes/find_5.inc"


#include "StringTypes/find_6.inc"


#include "StringTypes/find_7.inc"


#include "StringTypes/rfind.inc"


#include "StringTypes/rfind_1.inc"


#include "StringTypes/rfind_2.inc"


#include "StringTypes/rfind_3.inc"


#include "StringTypes/rfind_4.inc"


#include "StringTypes/rfind_5.inc"


#include "StringTypes/rfind_6.inc"


#include "StringTypes/rfind_7.inc"


// ---- CharString static 工厂 ----

#include "StringTypes/concat.inc"


#include "StringTypes/concat_1.inc"


#include "StringTypes/concat_2.inc"


#include "StringTypes/concat_3.inc"


#include "StringTypes/concat_4.inc"


#include "StringTypes/concat_5.inc"


#include "StringTypes/concat_6.inc"


#include "StringTypes/concat_7.inc"


#include "StringTypes/concat_8.inc"


#include "StringTypes/pattern.inc"


#include "StringTypes/format.inc"


#include "StringTypes/vformat.inc"


#include "StringTypes/tokenize.inc"


#include "StringTypes/tokenize_1.inc"


#include "StringTypes/tokenizeNewLine.inc"


#include "StringTypes/join.inc"


#include "StringTypes/compare.inc"


#include "StringTypes/compare_1.inc"


#include "StringTypes/compare_2.inc"


#include "StringTypes/compareNoCase.inc"


#include "StringTypes/compareNoCase_1.inc"


#include "StringTypes/compareNoCase_2.inc"


// ---- WideString 生命周期与数据访问 ----

#include "StringTypes/WideString.inc"


#include "StringTypes/WideString_1.inc"


#include "StringTypes/WideString_2.inc"


#include "StringTypes/WideString_3.inc"


#include "StringTypes/WideString_4.inc"


#include "StringTypes/WideString_5.inc"


#include "StringTypes/WideString_6.inc"


#include "StringTypes/dtor_WideString.inc"


#include "StringTypes/c_str_1.inc"


#include "StringTypes/length_1.inc"


#include "StringTypes/isEmpty_1.inc"


#include "StringTypes/isWhiteSpace_1.inc"


#include "StringTypes/getHash_1.inc"


#include "StringTypes/getAt_1.inc"


#include "StringTypes/setAt_1.inc"


#include "StringTypes/startsWith_1.inc"


#include "StringTypes/endsWith_1.inc"


#include "StringTypes/front_1.inc"


#include "StringTypes/back_1.inc"


#include "StringTypes/begin_1.inc"


#include "StringTypes/end_1.inc"


#include "StringTypes/assign_4.inc"


#include "StringTypes/assign_5.inc"


#include "StringTypes/assign_6.inc"


#include "StringTypes/assign_7.inc"


#include "StringTypes/clear_1.inc"


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


#include "StringTypes/replaceData_1.inc"


#include "StringTypes/getData_1.inc"


// ---- WideString::find / rfind ----

#include "StringTypes/find_8.inc"


#include "StringTypes/find_9.inc"


#include "StringTypes/find_10.inc"


#include "StringTypes/find_11.inc"


#include "StringTypes/find_12.inc"


#include "StringTypes/find_13.inc"


#include "StringTypes/find_14.inc"


#include "StringTypes/find_15.inc"


#include "StringTypes/rfind_8.inc"


#include "StringTypes/rfind_9.inc"


#include "StringTypes/rfind_10.inc"


#include "StringTypes/rfind_11.inc"


#include "StringTypes/rfind_12.inc"


#include "StringTypes/rfind_13.inc"


#include "StringTypes/rfind_14.inc"


#include "StringTypes/rfind_15.inc"


// ---- WideString static 工厂 ----

#include "StringTypes/concat_9.inc"


#include "StringTypes/concat_10.inc"


#include "StringTypes/concat_11.inc"


#include "StringTypes/concat_12.inc"


#include "StringTypes/concat_13.inc"


#include "StringTypes/concat_14.inc"


#include "StringTypes/concat_15.inc"


#include "StringTypes/concat_16.inc"


#include "StringTypes/concat_17.inc"


#include "StringTypes/pattern_1.inc"


#include "StringTypes/format_1.inc"


#include "StringTypes/vformat_1.inc"


#include "StringTypes/tokenize_2.inc"


#include "StringTypes/tokenize_3.inc"


#include "StringTypes/tokenizeNewLine_1.inc"


#include "StringTypes/join_1.inc"


#include "StringTypes/compare_3.inc"


#include "StringTypes/compare_4.inc"


#include "StringTypes/compare_5.inc"


#include "StringTypes/compareNoCase_3.inc"


#include "StringTypes/compareNoCase_4.inc"


#include "StringTypes/compareNoCase_5.inc"


#include "StringTypes/isuspace.inc"


#include "StringTypes/isupunct.inc"


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


#include "StringTypes/ss_vswprintf.inc"


#include "StringTypes/ss_strcpy.inc"


#include "StringTypes/ss_strcat.inc"


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
