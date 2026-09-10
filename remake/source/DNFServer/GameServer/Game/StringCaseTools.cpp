// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StringCaseTools/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _Z20readStringFromBufferPciRS_ 的前提，P338/N123）
#include <string.h>   // memcpy / strlen
#include <string>
#include <wchar.h>    // towlower/towupper
#include <wctype.h>   // towlower/towupper

// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _Z20readStringFromBufferPciRS_ ⇒
// 该重载定义须为本 TU 首个强符号（此前 lower 定义在前 ⇒ 键名错为
// _GLOBAL__I__Z5lowerRK10CharString）。readWordFromBuffer 是 inline（weak，不参与
// 选名）故一并前置。纯前置定义顺序，符号集合/函数体零变化。
// [N·GI 体跨版本差，不可消；Main 2026-09-10 裁决保留] ORIG GI（nm t 0x08ad00a0）
// 体为 9 条 -O2 前端内联形：直接 `call _ZNSt8ios_base4InitC1Ev` + `__cxa_atexit`
// （static_init 被内联）；我方 4.4 -O0 恒为 7 条「call
// _Z41__static_initialization_and_destruction_0ii」标准形（与 TimerQueue ORIG
// 0x0863baef 逐条同形，互为佐证）⇒ compare 判 GI 本体 DIFF，属跨版本前端差、
// 源码层不可控。勿再试形（改该 TU 编译选项会破坏其余 14 个已对齐函数）。
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
#include "StringCaseTools/readStringFromBuffer.inc"
}

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

// _Z20readStringFromBufferRSsRPc 0x08ad0460：
// 读 ushort 长度 n（buf 前移 2），string resize(n+1) 后 assign n 字节，buf 前进 n。
// [b28d-28] DIFF | ORIG 0x08ad0460 | 同上 N82/P75：ORIG ebx/esi/edi 缓存 +
// `mov %si,0x9501a1a`（readWordFromBuffer 局部 static，weak 合并）；4.4 槽化
// n（-0xa）+ 逐次重读 0xc(%ebp)。语义等价。
#include "StringCaseTools/readStringFromBuffer_1.inc"
}
