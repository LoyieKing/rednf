// RestrictNameDeps.cpp —— restrict_inputting_name 域 + checkValidCharacName 真实定义。
// 逐符号证据：
//   - importUnicodeRangeData：W _ZN23restrict_inputting_name25RestrictInputtingNameData
//     22importUnicodeRangeDataEPKcPSt6vectorISt4pairIiiESaIS5_EE @0x08ac2eca sz 0x29e
//     （报告 global_constructors_keyed_to_…/unicodeRangeDataList_.md 全文已读）。
//   - C1 @0x08ac3168、isInRange @0x08ac31a0、IsNoneUnicodeRange op() @0x08ac321c、
//     IsNoneUnicodeRange C1 @0x08ac3240、any_of/find_if/none_of 实例族
//     （W @0x08ac3253/0x08ac3278/0x08ac32a0）、isWideStrInUnicodeRange 报告全文已读。
//   - isUtf8StrInUnicodeRange 报告全文已读（WideString + convertFromUtf8 链）。
//   - checkValidCharacName @0x0820582b（报告缺失，objdump 全文转写）。

#include "RestrictNameDeps.h"
#include "StringTypes.h"          // WideString
#include "StringResource.h"       // convertFromUtf8
#include "DNFLexWrapper.h"        // loadRDARScriptFile/ScanType/ScanInt 权威声明
#include "LogManager.h"
#include <string.h>               // strlen
#include <wchar.h>                // wcslen
#include <algorithm>              // std::any_of（ORIG W _ZSt6any_ofIPKw…EEbT_S4_T0_ @0x08ac3253）
#include "CDataManager.h"        // G_CDataManager()/hasSpecialCharacter 族

namespace restrict_inputting_name
{

std::vector<std::pair<int, int> > RestrictInputtingNameData::unicodeRangeDataList_;
char RestrictInputtingNameData::isLoaded_ = 0;

// ORIG W 0x08ac2eca sz 0x29e 忠实转写（成员版；结构与 throw/catch/logFormat
// 路径全保留）：外层 for(;;)：seek 循环丢 token 直至 "[allow unicode range]"；
// body 循环里 "[/allow unicode range]" → out=tmp 并重回 seek；"[range]" →
// ScanInt 左右值入 tmp；其它 token 忽略。任一 ScanType 失败/越界抛 const char*，
// 被 catch 记日志后返 false。
bool RestrictInputtingNameData::importUnicodeRangeData(
    const char* path, std::vector<std::pair<int, int> >* out)
{
    if (!loadRDARScriptFile(path, ""))
        return false;
    std::string token;                       // ORIG +0x53（外层循环前构造一次）
    for (;;)
    {
        try
        {
            // seek "[allow unicode range]"（ORIG jmp 回 +0x61）
            for (;;)
            {
                if (!ScanType(token, true))
                    throw "[allow unicode range] 태그가 아님\n";
                if (token == "[allow unicode range]")
                    break;
            }
            std::vector<std::pair<int, int> > ranges;   // ORIG +0xc1 每轮新建
            for (;;)
            {
                if (!ScanType(token, true))
                    throw "태그가 와야 할 자리에 다른 값이 파싱 됨\n";
                if (token == "[/allow unicode range]")
                {
                    *out = ranges;                  // ORIG vector::operator= 复制
                    break;                          // 重回 seek（ORIG jmp +0x61）
                }
                if (token != "[range]")
                    continue;
                int left = 0;
                int right = 0;
                if (!ScanInt(&left))
                    throw "range의 left값을 읽어 오다 실패\n";
                if (!ScanInt(&right))
                    throw "range의 right값을 읽어 오다 실패\n";
                ranges.push_back(std::make_pair(left, right));
            }
        }
        catch (const char* message)
        {
            LogManager::logFormat(0, "../../RestrictInputtingName.cpp",
                                  __FUNCTION__, 0x48, "%s", message);
            return false;
        }
    }
    // ORIG 无正常退出路径（正常返回仅经 catch→return false）。
    return false;
}

RestrictInputtingNameData::RestrictInputtingNameData()
{
    static const char* scriptPath =
        "etc\\RestrictNameUnicodeRangeTable.etc";  // .rodata 0x08e2e68c 实测
    if (!isLoaded_)                                // ORIG movzbl+xor $1/test
    {
        isLoaded_ = (char)(unsigned char)this->importUnicodeRangeData(
            scriptPath, &unicodeRangeDataList_);
    }
}

// ORIG W 0x08ac31a0：for i<size: if (first<=ch && ch<=second) return true；
// ORIG 用 vector::operator[](i) 两次取 first/second——保持成员访问形态。
bool RestrictInputtingNameData::isInRange(wchar_t ch) const
{
    for (unsigned int i = 0; i < unicodeRangeDataList_.size(); ++i)
    {
        bool inRange =
            (unicodeRangeDataList_[i].first <= (int)ch &&
             (int)ch <= unicodeRangeDataList_[i].second);
        if (inRange)
            return true;
    }
    return false;
}

bool IsNoneUnicodeRange::operator()(wchar_t ch) const
{
    return !isInRange(ch);
}

// ORIG T 0x08ac2de4（isWideStrInUnicodeRange.md 全文）：
//   IsNoneUnicodeRange 构造（call 0x08ac3240）→ wcslen（call 0x0807e130）结果落
//   局部槽 -0x10（P0：具名 size_t 局部）→ any_of(str, str+len, pred)（call
//   0x08ac3253）→ 取反返回。any_of 实例 ORIG W _ZSt6any_ofIPKw
//   N23restrict_inputting_name18IsNoneUnicodeRangeEEbT_S4_T0_ @0x08ac3253
//   （c6root libstdc++ 4.4.7 bits/stl_algo.h:778 有该模板，可实例化）。
bool isWideStrInUnicodeRange(const wchar_t* str)
{
    IsNoneUnicodeRange pred;
    size_t len = wcslen(str);
    return !std::any_of(str, str + len, pred);
}

// ORIG T 0x08ac2d6c sz 0x78（isUtf8StrInUnicodeRange.md 全文）：
// WideString 构造 → convertFromUtf8(str,(int)strlen,str 的宽度) → c_str 判定 → 析构返回。
bool isUtf8StrInUnicodeRange(const char* str)
{
    WideString wide;
    convertFromUtf8(str, (int)strlen(str), &wide);
    return isWideStrInUnicodeRange(wide.c_str());
}

}  // namespace restrict_inputting_name

// ORIG T 0x0820582b _Z20checkValidCharacNamePKc sz 0x6c（objdump 全文转写）：
// G_CDataManager()->hasSpecialCharacter / hasPreventString / hasPreventStringName 三连。
bool checkValidCharacName(const char* name)
{
    if (G_CDataManager()->hasSpecialCharacter(name))
        return false;
    if (G_CDataManager()->hasPreventString(name))
        return false;
    if (G_CDataManager()->hasPreventStringName(name))
        return true;
    return false;
}

// ORIG 函数无正常退出路径（外层 for(;;) 永不离开；正常返回仅经 catch→return false）。
// 此行不可达，仅为抑制编译器 non-void 返回警告。
