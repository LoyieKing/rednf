// ============================================================================
// StringConvert.cpp —— CharString / WideString 自由 insert/replace/toUnicode/
// convertTo* 函数（G4 域字符串自由函数）。
//
// 全部按 ORIG df_game_r 反汇编逐函数转写
// （docs/class_func_reports/_lt_global_gt_/{insert,replace,toUnicode,
//  convertToWideString,convertToCharString}_*.md）：
//   insert CharString 4 重载      0x08ad76ce..0x08ad7841
//   insert WideString 4 重载      0x08ada71e..0x08ada956
//   replace CharString 4 重载     0x08ad7afa..0x08ad7dec
//   replace WideString 4 重载     0x08adab85..0x08adae9d
//   toUnicode 8 重载              0x08adf0f6..0x08adf3de
//   convertToWideString 5 重载    0x08adf3f9..0x08adf6bd
//   convertToCharString(RK10CharString/RK10WideStringj) 0x08adf6c0/0x08adf65b
// convertToCharString(PKwj) 0x08adf57f 归 StringResource.cpp（本 TU 依赖其
// 转发入口）；convertToCharString(PKw/RK10WideString) 亦在 StringResource.cpp。
//
// toUnicode 的 ORIG 以 %gs TLS 访问 10×0x1000 宽字符环形缓冲 + 槽索引
// （(idx+1)%10），重建以同 TU static __thread 缓冲迁移（TU 局部，无跨 TU
// ODR 冲突；与 SyncScriptDeps.cpp / UnicodeConvert.cpp 的缓冲互不共享，
// 与 ORIG 各 TU 局部缓冲的语义一致）。
// ============================================================================

#include "StringTypes.h"

#include <stdlib.h>   // mbstowcs
#include <string.h>   // memcmp / memcpy / strlen
#include <wchar.h>    // wcslen

// ORIG 单元内 static _ZL9gCodePage（bss 0x9501e90，全二进制仅读无写，恒 0）；
// convertToWideString(PKc) 以内存读取传入（AE 口径下与 ORIG 形态一致）。
static unsigned int gCodePage;

// ORIG 0x08adf57f 定义于 StringResource.cpp；本 TU 显式声明，确保
// convertToCharString(const WideString&, uint) 转发到 PKwj 重载
// （否则 WideString(const wchar_t*) 隐式转换会选 RK10WideStringj 自递归）。
CharString convertToCharString(const wchar_t* src, unsigned int srcLength);

// ---------------------------------------------------------------------------
// getMbcsBuffer / getUnicodeBuffer / getTCharBuffer —— ORIG 0x08adea35/54/73
// （UnicodeConvert.cpp 域）：TLS 环形缓冲当前槽指针（shl 12/14 槽步长实证）。
// 缓冲与槽索引均 TU 局部（与 SyncScriptDeps.cpp 的 toMbcs 缓冲不共享；
// ORIG 各 Unicode 转换 TU 本就各自维护 TLS 缓冲）。
// [b28 BM2 判词·N·TLS 布局伪差] 三件 NEAR(2)：fresh .o fn_diff_tu 归一 0 差
// （/tmp/tu_StringConvert.o 实证）；全二进制残差 = %gs 相对偏移立即数（ORIG
// index 落 TLS 块 -0x22028 vs 本 TU 块内布局不同）+ lea 符号注记——TLS 块内
// 布局跨工具链产物，源码不可控（P155 TLS %gs 访问同形豁免族）。语义逐条等价。
// ---------------------------------------------------------------------------
static __thread char gMbcsBuffer[10][0x1000];
static __thread int gMbcsBufferIndex;
// ORIG shl $0xe：槽字节步长 0x4000 = wchar_t[0x1000]（0x08adea54 实证）。
static __thread wchar_t gUnicodeBuffer[10][0x1000];
static __thread int gUnicodeBufferIndex;
static __thread char gTCharBuffer[10][0x1000];
static __thread int gTCharBufferIndex;

// [b28d-L1] IDENTICAL/AE（routed 口径 check_tu_routed）：ORIG 0x08adea35；残余仅 %gs TLS 块内偏移立即数（跨工具链 TLS 布局，P155 同形豁免）。
char* getMbcsBuffer()          // ORIG T 0x08adea35
{
    return gMbcsBuffer[gMbcsBufferIndex];
}
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adea54；残余仅 %gs TLS 布局立即数（P155 豁免）。
wchar_t* getUnicodeBuffer()    // ORIG T 0x08adea54
{
    return gUnicodeBuffer[gUnicodeBufferIndex];
}
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adea73；残余仅 %gs TLS 布局立即数（P155 豁免）。
char* getTCharBuffer()         // ORIG T 0x08adea73
{
    return gTCharBuffer[gTCharBufferIndex];
}
int getMbcsBufferSize()        // ORIG T 0x08adea92
{
    return 0x1000;
}
int getUnicodeBufferSize()     // ORIG T 0x08adea9c
{
    return 0x4000;
}
int getTCharBufferSize()       // ORIG T 0x08adeaa6
{
    return 0x1000;
}

// ---------------------------------------------------------------------------
// insert —— CharString
// ---------------------------------------------------------------------------

// _Z6insertRK10CharStringiPKci 0x08ad771a
// [b28 判词·N] AE-NEAR(2)：第二 memcpy 的 srcLength(0x18)/src(0x10) 装载——4.1 先
// 读远参入 %edx 再读近参入 %ecx，4.4 恒先近后远（P209 附 memcpy 装载跨版本定论，
// 探针 e19；N82附 多实参寄存器对调）。2 行相邻互换，禁 register 绑定（P0/N96）。
// [b28d-L1] 修法：ORIG 0x08ad771a 第三段 dest 为 `mov srcLength,%edx; mov pos,%ecx; lea (%ecx,%edx,1)`；未分组 `+ pos + srcLength` 使 4.4 反序，改 `+ (pos + srcLength)` 后 AE（探针 /tmp/b28d_l1/probe/p_ins.cpp f1/f3）。
CharString insert(const CharString& str, int pos, const char* src, int srcLength)
{
    if (str.length() == 0)
        return CharString(src, srcLength);
    if (srcLength == 0)
        return CharString(str);
    CharStringData* newData = CharStringData::createTerminated(str.length() + srcLength);
    memcpy(newData->getBuffer(), str.c_str(), pos);
    memcpy(newData->getBuffer() + pos, src, srcLength);
    // [b28d-L1] 修法：第三段 dest 写 `+ (pos + srcLength)` 显式分组——ORIG
    // 0x08ad78c6 段为 `mov srcLength,%edx; mov pos,%ecx; lea (%ecx,%edx,1)`，
    // 未分组的 `+ pos + srcLength` 使 4.4 反序（探针 /tmp/b28d_l1/probe/p_ins.cpp
    // f1 vs f3 实证）。语义等价（指针+整数结合律）。
    memcpy(newData->getBuffer() + (pos + srcLength), str.c_str() + pos,
           str.length() - pos);
    return CharString(newData);
}

// _Z6insertRK10CharStringiPKc 0x08ad76ce
CharString insert(const CharString& str, int pos, const char* src)
{
    return insert(str, pos, src, static_cast<int>(strlen(src)));
}

// _Z6insertRK10CharStringiS1_ 0x08ad7844（ORIG 三分支：str 空→src 拷贝；
// src 空→str 拷贝；否则转发 PKci）
CharString insert(const CharString& str, int pos, const CharString& src)
{
    if (str.length() == 0)
        return CharString(src);
    if (src.length() == 0)
        return CharString(str);
    return insert(str, pos, src.c_str(), src.length());
}

// _Z6insertRK10CharStringic 0x08ad78ea（ORIG 内联三分支：str 空→CharString(c)；
// c==0→str 拷贝；否则 createTerminated(len+1) + memcpy 两段 + 单字节写）
CharString insert(const CharString& str, int pos, char src)
{
    if (str.length() == 0)
        return CharString(src);
    if (src == '\0')
        return CharString(str);
    CharStringData* newData = CharStringData::createTerminated(str.length() + 1);
    memcpy(newData->getBuffer(), str.c_str(), pos);
    newData->getBuffer()[pos] = src;
    memcpy(newData->getBuffer() + pos + 1, str.c_str() + pos, str.length() - pos);
    return CharString(newData);
}

// ---------------------------------------------------------------------------
// insert —— WideString
// ---------------------------------------------------------------------------

// _Z6insertRK10WideStringiPKwi 0x08ada76a
// [b28 判词·N] 同 CharString 版：memcpy 实参装载序 4.1/4.4 互换（P209 附/N82附），
// 2 行寄存器对调，禁绑定形（P0）。
// [b28d-L1] 修法：同 CharString 版（ORIG 0x08ada76a 段装载序），dest 改 `+ (pos + srcLength)` 后 AE。
WideString insert(const WideString& str, int pos, const wchar_t* src, int srcLength)
{
    if (str.length() == 0)
        return WideString(src, srcLength);
    if (srcLength == 0)
        return WideString(str);
    WideStringData* newData = WideStringData::createTerminated(str.length() + srcLength);
    memcpy(newData->getBuffer(), str.c_str(), pos * sizeof(wchar_t));
    memcpy(newData->getBuffer() + pos, src, srcLength * sizeof(wchar_t));
    // [b28d-L1] 修法：同 CharString 版（ORIG 0x08ada8e6 段装载序）。
    memcpy(newData->getBuffer() + (pos + srcLength), str.c_str() + pos,
           (str.length() - pos) * sizeof(wchar_t));
    return WideString(newData);
}

// _Z6insertRK10WideStringiPKw 0x08ada71e
WideString insert(const WideString& str, int pos, const wchar_t* src)
{
    return insert(str, pos, src, static_cast<int>(wcslen(src)));
}

// _Z6insertRK10WideStringiS1_ 0x08ada8b0
WideString insert(const WideString& str, int pos, const WideString& src)
{
    if (str.length() == 0)
        return WideString(src);
    if (src.length() == 0)
        return WideString(str);
    return insert(str, pos, src.c_str(), src.length());
}

// _Z6insertRK10WideStringiw 0x08ada956
WideString insert(const WideString& str, int pos, wchar_t src)
{
    if (str.length() == 0)
        return WideString(src);
    if (src == 0)
        return WideString(str);
    WideStringData* newData = WideStringData::createTerminated(str.length() + 1);
    memcpy(newData->getBuffer(), str.c_str(), pos * sizeof(wchar_t));
    newData->getBuffer()[pos] = src;
    memcpy(newData->getBuffer() + pos + 1, str.c_str() + pos,
           (str.length() - pos) * sizeof(wchar_t));
    return WideString(newData);
}

// ---------------------------------------------------------------------------
// replace —— CharString
// ---------------------------------------------------------------------------

// _Z7replaceRK10CharStringPKciS3_i 0x08ad7c36
CharString replace(const CharString& str, const char* pat, int patLength,
                   const char* target, int targetLength)
{
    // 变量全部前置声明（ORIG 栈槽顺序：strBuffer/-0x34、strLength/-0x30、
    // replaceCount/-0x2c、wordDelta/-0x28、nowLength/-0x24、newLength/-0x20、
    // newData/-0x1c、newBuffer/-0x18、i/-0x14、i2/-0x10、ni/-0xc）。
    const char* strBuffer;
    int strLength;
    int replaceCount;
    int wordDelta;
    int nowLength;
    int newLength;
    CharStringData* newData;
    char* newBuffer;
    int i;
    int i2;
    int ni;

    if (patLength == 0)
        return CharString(str);
    strBuffer = str.c_str();
    strLength = str.length();
    replaceCount = 0;
    i = 0;
    while (i <= strLength - patLength)
    {
        if (memcmp(strBuffer + i, pat, patLength) == 0)
        {
            ++replaceCount;
            i += patLength;
        }
        else
        {
            ++i;
        }
    }
    if (replaceCount == 0)
        return CharString(str);
    wordDelta = targetLength - patLength;
    nowLength = strLength;
    newLength = wordDelta * replaceCount + strLength;
    if (newLength == 0)
    {
        return CharString();
    }
    newData = CharStringData::createTerminated(newLength);
    newBuffer = newData->getBuffer();
    i2 = 0;
    ni = 0;
    while (i2 < strLength)
    {
        if (i2 <= strLength - patLength &&
            memcmp(strBuffer + i2, pat, patLength) == 0)
        {
            memcpy(newBuffer + ni, target, targetLength);
            i2 += patLength;
            ni += targetLength;
        }
        else
        {
            newBuffer[ni] = strBuffer[i2];
            ++ni;
            ++i2;
        }
    }
    return CharString(newData);
}

// _Z7replaceRK10CharStringPKcS3_ 0x08ad7bd7
CharString replace(const CharString& str, const char* pat, const char* target)
{
    return replace(str, pat, static_cast<int>(strlen(pat)),
                   target, static_cast<int>(strlen(target)));
}

// _Z7replaceRK10CharStringS1_S1_ 0x08ad7def
CharString replace(const CharString& str, const CharString& pat,
                   const CharString& target)
{
    return replace(str, pat.c_str(), pat.length(), target.c_str(), target.length());
}

// _Z7replaceRK10CharStringcc 0x08ad7afa
CharString replace(const CharString& str, char pat, char target)
{
    if (str.length() == 0)
        return CharString();
    CharStringData* newData = CharStringData::createTerminated(str.length());
    char* newBuffer = newData->getBuffer();
    const char* strBuffer = str.c_str();
    const int strLength = str.length();
    for (int i = 0; i < strLength; ++i)
    {
        if (strBuffer[i] == pat)
            newBuffer[i] = target;
        else
            newBuffer[i] = strBuffer[i];
    }
    return CharString(newData);
}

// ---------------------------------------------------------------------------
// replace —— WideString
// ---------------------------------------------------------------------------

// _Z7replaceRK10WideStringPKwiS3_i 0x08adacbe
// [b28d-L1] N 封顶（P75 实参装载时刻）：ORIG 0x08adab85 中 receiver/实参装载块位置与 4.4 不同（real diff 30 全为装载序/寄存器形态）。P167 CLEAR（11/11）⇒ 语义等价；装载时刻跨版本不可择。
WideString replace(const WideString& str, const wchar_t* pat, int patLength,
                   const wchar_t* target, int targetLength)
{
    const wchar_t* strBuffer;
    int strLength;
    int replaceCount;
    int wordDelta;
    int nowLength;
    int newLength;
    WideStringData* newData;
    wchar_t* newBuffer;
    int i;
    int i2;
    int ni;

    if (patLength == 0)
        return WideString(str);
    strBuffer = str.c_str();
    strLength = str.length();
    replaceCount = 0;
    i = 0;
    while (i <= strLength - patLength)
    {
        if (memcmp(strBuffer + i, pat, patLength * sizeof(wchar_t)) == 0)
        {
            ++replaceCount;
            i += patLength;
        }
        else
        {
            ++i;
        }
    }
    if (replaceCount == 0)
        return WideString(str);
    wordDelta = targetLength - patLength;
    nowLength = strLength;
    newLength = wordDelta * replaceCount + strLength;
    if (newLength != 0)
    {
        newData = WideStringData::createTerminated(newLength);
        newBuffer = newData->getBuffer();
        i2 = 0;
        ni = 0;
        while (i2 < strLength)
        {
            if (i2 > strLength - patLength ||
                memcmp(strBuffer + i2, pat, patLength * sizeof(wchar_t)) != 0)
            {
                newBuffer[ni] = strBuffer[i2];
                ++ni;
                ++i2;
            }
            else
            {
                memcpy(newBuffer + ni, target, targetLength * sizeof(wchar_t));
                i2 += patLength;
                ni += targetLength;
            }
        }
        return WideString(newData);
    }
    return WideString();
}

// _Z7replaceRK10WideStringPKwS3_ 0x08adac5f
WideString replace(const WideString& str, const wchar_t* pat, const wchar_t* target)
{
    return replace(str, pat, static_cast<int>(wcslen(pat)),
                   target, static_cast<int>(wcslen(target)));
}

// _Z7replaceRK10WideStringS1_S1_ 0x08adae9d
WideString replace(const WideString& str, const WideString& pat,
                   const WideString& target)
{
    return replace(str, pat.c_str(), pat.length(), target.c_str(), target.length());
}

// _Z7replaceRK10WideStringww 0x08adab85
WideString replace(const WideString& str, wchar_t pat, wchar_t target)
{
    if (str.length() == 0)
        return WideString();
    WideStringData* newData = WideStringData::createTerminated(str.length());
    wchar_t* newBuffer = newData->getBuffer();
    const wchar_t* strBuffer = str.c_str();
    const int strLength = str.length();
    for (int i = 0; i < strLength; ++i)
    {
        if (strBuffer[i] == pat)
            newBuffer[i] = target;
        else
            newBuffer[i] = strBuffer[i];
    }
    return WideString(newData);
}

// ---------------------------------------------------------------------------
// toUnicode —— 恒等 / c_str() 便捷重载
// ---------------------------------------------------------------------------

// _Z9toUnicodePKw 0x08adf254：恒等返回。
const wchar_t* toUnicode(const wchar_t* src)
{
    return src;
}

// _Z9toUnicodeRKSbIwSt11char_traitsIwESaIwEE 0x08adf1a0：c_str()。
const wchar_t* toUnicode(const std::wstring& src)
{
    return src.c_str();
}

// _Z9toUnicodePKwRSbIwSt11char_traitsIwESaIwEE 0x08adf3df：sDst = src。
void toUnicode(const wchar_t* src, std::wstring& dst)
{
    dst = src;
}

// _Z9toUnicodeRKSbIwSt11char_traitsIwESaIwEERS2_ 0x08adf315：sDst = sSrc。
void toUnicode(const std::wstring& src, std::wstring& dst)
{
    dst = src;
}

// ---------------------------------------------------------------------------
// toUnicode —— mbstowcs 到 TLS 环形缓冲
// ---------------------------------------------------------------------------

// _Z9toUnicodePKc 0x08adf1b3
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adf1b3；残余仅 %gs TLS 布局立即数（P155 豁免）。
const wchar_t* toUnicode(const char* src)
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src, 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
    return gUnicodeBuffer[curindex];
}

// _Z9toUnicodeRKSs 0x08adf0f6
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adf0f6；残余仅 %gs TLS 布局立即数（P155 豁免）。
const wchar_t* toUnicode(const std::string& src)
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src.c_str(), 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
    return gUnicodeBuffer[curindex];
}

// _Z9toUnicodePKcRSbIwSt11char_traitsIwESaIwEE 0x08adf32f
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adf32f；残余仅 %gs TLS 布局立即数（P155 豁免）。
void toUnicode(const char* src, std::wstring& dst)
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src, 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}

// _Z9toUnicodeRKSsRSbIwSt11char_traitsIwESaIwEE 0x08adf25c
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x08adf25c；残余仅 %gs TLS 布局立即数（P155 豁免）。
void toUnicode(const std::string& src, std::wstring& dst)
{
    const int curindex = gUnicodeBufferIndex;
    const std::size_t nRet = mbstowcs(gUnicodeBuffer[curindex], src.c_str(), 0x1000);
    gUnicodeBuffer[curindex][nRet] = 0;
    dst = gUnicodeBuffer[curindex];
    gUnicodeBufferIndex = (gUnicodeBufferIndex + 1) % 10;
}

// ---------------------------------------------------------------------------
// convertToWideString —— 默认 gCodePage=0（ORIG 单元内 _ZL9gCodePage 恒 0，
// 无写方）；codePage 参数在 ORIG 中仅作 mbstowcs 长度界使用。
// ---------------------------------------------------------------------------

// _Z19convertToWideStringPKcj 0x08adf42c
WideString convertToWideString(const char* src, unsigned int codePage)
{
    (void)codePage;
    const std::size_t size = mbstowcs(NULL, src, 0);
    if (size == static_cast<std::size_t>(-1))
        return WideString();
    const int length = static_cast<int>(size >> 2);
    if (length == 0)
        return WideString();
    WideStringData* newData = WideStringData::create(length);
    mbstowcs(newData->getBuffer(), src, (length + 1) * 4);
    return WideString(newData);
}

// _Z19convertToWideStringPKc 0x08adf3f9：gCodePage 恒 0。
WideString convertToWideString(const char* src)
{
    return convertToWideString(src, gCodePage);
}

// _Z19convertToWideStringRK10CharString 0x08adf4d7
WideString convertToWideString(const CharString& src)
{
    return convertToWideString(src.c_str());
}

// _Z19convertToWideStringRK10CharStringj 0x08adf50e
WideString convertToWideString(const CharString& src, unsigned int codePage)
{
    return convertToWideString(src.c_str(), codePage);
}

// _Z19convertToWideStringRK10WideString 0x08adf699：拷贝构造。
WideString convertToWideString(const WideString& src)
{
    return WideString(src);
}

// ---------------------------------------------------------------------------
// convertToCharString —— 缺失重载（PKwj 归 StringResource.cpp）
// ---------------------------------------------------------------------------

// _Z19convertToCharStringRK10CharString 0x08adf6c0：拷贝构造。
CharString convertToCharString(const CharString& src)
{
    return CharString(src);
}

// _Z19convertToCharStringRK10WideStringj 0x08adf65b：转发 c_str() 到 PKwj。
CharString convertToCharString(const WideString& src, unsigned int codePage)
{
    return convertToCharString(src.c_str(), codePage);
}
