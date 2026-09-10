// ============================================================================
// StringCaseTools.cpp —— CharString / WideString 大小写/裁剪/反转/删除自由函数
// 与 readStringFromBuffer 缓冲读取工具。
//
// 全部按 ORIG df_game_r 反汇编逐函数转写（OnlyOrigBatch7 批次）：
//   lower   CharString 0x08ad8062 / WideString 0x08adb119
//   upper   CharString 0x08ad8120 / WideString 0x08adb1d9
//   reverse CharString 0x08ad81de / WideString 0x08adb299
//   remove  CharString 0x08ad7a01 / WideString 0x08adaa7a
//   trim    CharString 0x08ad84c0 / WideString 0x08adb596（trimLeft(trimRight)）
//   trimLeft  CharString 0x08ad82a9 / WideString 0x08adb369
//   trimRight CharString 0x08ad83c0 / WideString 0x08adb491
//   readStringFromBuffer(char*,int,char*&)   0x08ad0130
//   readStringFromBuffer(std::string&,char*&) 0x08ad0460
// ============================================================================

#include "StringTypes.h"

#include <ctype.h>    // tolower/toupper/isspace
#include <string.h>   // memcpy / strlen
#include <string>
#include <wchar.h>    // towlower/towupper
#include <wctype.h>   // towlower/towupper

// ---------------------------------------------------------------------------
// lower / upper
// ---------------------------------------------------------------------------

// _Z5lowerRK10CharString 0x08ad8062
CharString lower(const CharString& str)
{
    if (str.length() == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(str.length());
    const char* src = str.c_str();
    char* buf = newData->getBuffer();
    for (int i = 0; i < str.length(); ++i)
        buf[i] = (char)tolower((int)src[i]);
    return CharString(newData);
}

// _Z5lowerRK10WideString 0x08adb119
WideString lower(const WideString& str)
{
    if (str.length() == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(str.length());
    const wchar_t* src = str.c_str();
    wchar_t* buf = newData->getBuffer();
    for (int i = 0; i < str.length(); ++i)
        buf[i] = (wchar_t)towlower(src[i]);
    return WideString(newData);
}

// _Z5upperRK10CharString 0x08ad8120
CharString upper(const CharString& str)
{
    if (str.length() == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(str.length());
    const char* src = str.c_str();
    char* buf = newData->getBuffer();
    for (int i = 0; i < str.length(); ++i)
        buf[i] = (char)toupper((int)src[i]);
    return CharString(newData);
}

// _Z5upperRK10WideString 0x08adb1d9
WideString upper(const WideString& str)
{
    if (str.length() == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(str.length());
    const wchar_t* src = str.c_str();
    wchar_t* buf = newData->getBuffer();
    for (int i = 0; i < str.length(); ++i)
        buf[i] = (wchar_t)towupper(src[i]);
    return WideString(newData);
}

// ---------------------------------------------------------------------------
// reverse
// ---------------------------------------------------------------------------

// _Z7reverseRK10CharString 0x08ad81de
CharString reverse(const CharString& str)
{
    if (str.length() == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(str.length());
    // ORIG：索引用缓存 strLength(-0x18)，循环测试重读 str.length()（iVar4）。
    const int strLength = str.length();
    const char* src = str.c_str();
    char* buf = newData->getBuffer();
    // 循环条件 `str.length() > i`（ORIG test 重读 length 且 `cmp i,length; setg`），
    // 体索引 `src[strLength - i - 1]` = (strLength-i)-1（ORIG esi 中介形）。
    int i = 0;
    while (str.length() > i)
    {
        buf[i] = src[strLength - i - 1];
        ++i;
    }
    return CharString(newData);
}

// _Z7reverseRK10WideString 0x08adb299
WideString reverse(const WideString& str)
{
    if (str.length() == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(str.length());
    const int strLength = str.length();
    const wchar_t* src = str.c_str();
    wchar_t* buf = newData->getBuffer();
    int i = 0;
    while (str.length() > i)
    {
        buf[i] = src[strLength - i - 1];
        ++i;
    }
    return WideString(newData);
}

// ---------------------------------------------------------------------------
// remove（pos 起删除 removeLength）
// ---------------------------------------------------------------------------

// _Z6removeRK10CharStringii 0x08ad7a01
// [b28d-28] AE | P269 指针加法分组修复：第二 memcpy 源写
// `c_str() + (pos + removeLength)` 复现 ORIG `mov 0x14,%edx; mov 0x10,%ecx;
// lea (%ecx,%edx,1),%edx` 的装载序（原 `+ pos + removeLength` 为 4.4 先近后远）。
CharString remove(const CharString& str, int pos, int removeLength)
{
    if (removeLength == 0)
        return CharString(str);
    if (str.length() == removeLength)
        return CharString();
    CharStringData* newData =
        CharStringData::createTerminated(str.length() - removeLength);
    // [b24ab] P48：ORIG 无 src/buf 具名局部，getBuffer/c_str 每用点重取
    // （两次 getBuffer、两次 c_str、两次 length 实调用）
    memcpy(newData->getBuffer(), str.c_str(), pos);
    memcpy(newData->getBuffer() + pos, str.c_str() + (pos + removeLength),
           str.length() - pos - removeLength);
    return CharString(newData);
}

// _Z6removeRK10WideStringii 0x08adaa7a
// [b28d-28] AE | 同 CharString 版：P269 指针加法分组 `c_str() + (pos + removeLength)`。
WideString remove(const WideString& str, int pos, int removeLength)
{
    if (removeLength == 0)
        return WideString(str);
    if (str.length() == removeLength)
        return WideString();
    WideStringData* newData =
        WideStringData::createTerminated(str.length() - removeLength);
    // [b24ab] P48：ORIG 无 src/buf 具名局部，getBuffer/c_str 每用点重取
    memcpy(newData->getBuffer(), str.c_str(), pos * sizeof(wchar_t));
    memcpy(newData->getBuffer() + pos, str.c_str() + (pos + removeLength),
           (str.length() - pos - removeLength) * sizeof(wchar_t));
    return WideString(newData);
}

// ---------------------------------------------------------------------------
// trimLeft / trimRight / trim
// ---------------------------------------------------------------------------

// _Z8trimLeftRK10CharString 0x08ad82a9
CharString trimLeft(const CharString& str) throw()
{
    if (str.length() == 0)
        return CharString();
    const char* src = str.c_str();
    const int end = str.length() - 1;
    int i = 0;
    for (; i <= end; ++i)
    {
        if (!isspace((int)src[i]))
        {
            if (i == 0)
                return CharString(str);
            CharStringData* newData =
                CharStringData::createTerminated(str.length() - i);
            memcpy(newData->getBuffer(), src + i, str.length() - i);
            return CharString(newData);
        }
    }
    return CharString();
}

// _Z8trimLeftRK10WideString 0x08adb369
WideString trimLeft(const WideString& str) throw()
{
    if (str.length() == 0)
        return WideString();
    const wchar_t* src = str.c_str();
    const int end = str.length() - 1;
    int i = 0;
    for (; i <= end; ++i)
    {
        if (!WideString::isuspace(src[i]))
        {
            if (i == 0)
                return WideString(str);
            WideStringData* newData =
                WideStringData::createTerminated(str.length() - i);
            memcpy(newData->getBuffer(), src + i,
                   (str.length() - i) * sizeof(wchar_t));
            return WideString(newData);
        }
    }
    return WideString();
}

// _Z9trimRightRK10CharString 0x08ad83c0
CharString trimRight(const CharString& str) throw()
{
    if (str.length() == 0)
        return CharString();
    const char* src = str.c_str();
    const int end = str.length() - 1;
    int i = end;
    for (; i >= 0; --i)
    {
        if (!isspace((int)src[i]))
        {
            if (i == end)
                return CharString(str);
            CharStringData* newData = CharStringData::createTerminated(i + 1);
            memcpy(newData->getBuffer(), src, i + 1);
            return CharString(newData);
        }
    }
    return CharString();
}

// _Z9trimRightRK10WideString 0x08adb491
WideString trimRight(const WideString& str) throw()
{
    if (str.length() == 0)
        return WideString();
    const wchar_t* src = str.c_str();
    const int end = str.length() - 1;
    int i = end;
    for (; i >= 0; --i)
    {
        if (!WideString::isuspace(src[i]))
        {
            if (i == end)
                return WideString(str);
            WideStringData* newData = WideStringData::createTerminated(i + 1);
            memcpy(newData->getBuffer(), src, (i + 1) * sizeof(wchar_t));
            return WideString(newData);
        }
    }
    return WideString();
}

// _Z4trimRK10CharString 0x08ad84c0
CharString trim(const CharString& str)
{
    return trimLeft(trimRight(str));
}

// _Z4trimRK10WideString 0x08adb596
WideString trim(const WideString& str)
{
    return trimLeft(trimRight(str));
}

// ---------------------------------------------------------------------------
// readStringFromBuffer（2 字节长度前缀缓冲读取）
// ---------------------------------------------------------------------------

// readWordFromBuffer —— [b28e-L5] 真缺口修复：ORIG 的 readStringFromBuffer 两重载里
// 都出现 `movzwl (%eax),%edi; mov %di,_ZZ18readWordFromBufferRPcE10wordBuffer`
// （0x08ad0130 段 / 0x08ad0460 段），而 ORIG 符号表**只有**该函数局部 static
// （nm: `09501a1a 00000002 V _ZZ18readWordFromBufferRPcE10wordBuffer`），
// **没有** `_Z18readWordFromBufferRPc` 独立函数符号（size 0 ⇒ 从不 out-of-line 发射）
// ⇒ 原码是「外部链接 inline + always_inline」的 16 位读取器，调用点全部前端展开。
// 我方原直写 `*(unsigned short*)buf; buf += 2;` 缺了这次 static 写入（真语义缺口：
// 该缓冲被同 TU 其它读取器共享）。据此补回，inline 为外部链接（mangled 与 ORIG 同，
// 不带 L 前缀）。
inline __attribute__((always_inline)) unsigned short readWordFromBuffer(char*& buf)
{
    static unsigned short wordBuffer;                 // ORIG 0x09501a1a（sz 2, V）
    wordBuffer = *reinterpret_cast<unsigned short*>(buf);   // mov (%eax),%reg; mov %reg16,global
    buf += 2;
    return wordBuffer;
}

// _Z20readStringFromBufferPciRS_ 0x08ad0130：
// 读 ushort 长度 n（buf 前移 2），拷贝 min(n, maxLen-1) 字节并以 NUL 收尾，
// buf 前进 n 字节。
// [b28d-28] DIFF | ORIG 0x08ad0130 | N82/P75：ORIG 以 ebx/esi/edi 缓存
// dst/maxLen/buf 与 n（`mov %di,_ZZ18readWordFromBufferRPcE10wordBuffer`——
// 4.1 inline 展开遗留的 only-ORIG weak 符号）；4.4 重读形参并把 n 槽化
// （-0xe）、min() 折 cmovg。源形不可造，语义等价。
void readStringFromBuffer(char* dst, int maxLen, char*& buf)
{
    // [b28e-L5] 改用 inline readWordFromBuffer（ORIG 在此点内联展开并写 static
    // wordBuffer）⇒ 复现 `movzwl (%eax),%edi; mov %di,0x9501a1a; add $0x2,%eax;
    // mov %eax,(%ebx)` 序列。
    const unsigned short n = readWordFromBuffer(buf);
    const int len = (int)n <= maxLen - 1 ? (int)n : maxLen - 1;
    memcpy(dst, buf, len);
    dst[len] = 0;
    buf += n;
}

// _Z20readStringFromBufferRSsRPc 0x08ad0460：
// 读 ushort 长度 n（buf 前移 2），string resize(n+1) 后 assign n 字节，buf 前进 n。
// [b28d-28] DIFF | ORIG 0x08ad0460 | 同上 N82/P75：ORIG ebx/esi/edi 缓存 +
// `mov %si,0x9501a1a`（readWordFromBuffer 局部 static，weak 合并）；4.4 槽化
// n（-0xa）+ 逐次重读 0xc(%ebp)。语义等价。
void readStringFromBuffer(std::string& str, char*& buf)
{
    // [b28e-L5] 同上（ORIG 此处 `mov %si,0x9501a1a`）。
    const unsigned short n = readWordFromBuffer(buf);
    str.resize(n + 1, 0);
    str.assign(buf, n);
    buf += n;
}
