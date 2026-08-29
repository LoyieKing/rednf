#ifndef GAME_RESTRICTNAMEDEPS_H_
#define GAME_RESTRICTNAMEDEPS_H_

// df_game_r restrict_inputting_name 域 + checkValidCharacName 权威声明
// （报告 docs/class_func_reports/restrict_inputting_name/{README,isUtf8StrInUnicodeRange,
//  isWideStrInUnicodeRange}.md 与 global_constructors_keyed_to_…/unicodeRangeDataList_.md；
//  缺独立报告的符号按 ORIG objdump 忠实转写，地址见各定义处注释）。
//
// 数据布局（nm/objdump 实测）：
//   B _ZN23restrict_inputting_name25RestrictInputtingNameData21unicodeRangeDataList_E
//     @0x09501830（vector<pair<int,int>>，sizeof 0xc）
//   B …isLoaded_ @0x0950183c（1 字节，初值 0）
//   V _ZZN…C1EvE10scriptPath @0x093f5678 → .rodata 0x08e2e68c
//     "etc\RestrictNameUnicodeRangeTable.etc"（objdump -s 实测）
// scriptPath 为 C1 函数内 static（"etc\RestrictNameUnicodeRangeTable.etc"）。

#include <string>
#include <utility>
#include <vector>

namespace restrict_inputting_name
{

class RestrictInputtingNameData
{
public:
    RestrictInputtingNameData();                       // ORIG W C1 0x08ac3168
    bool isInRange(wchar_t ch) const;                  // ORIG W _ZNK…9isInRangeEw 0x08ac31a0
    bool importUnicodeRangeData(const char* path,
                                std::vector<std::pair<int, int> >* out);
    // ORIG W _ZN23restrict_inputting_name25RestrictInputtingNameData22
    // importUnicodeRangeDataEPKcPSt6vectorISt4pairIiiESaIS5_EE @0x08ac2eca

    static std::vector<std::pair<int, int> > unicodeRangeDataList_;   // B 0x09501830
    static char isLoaded_;                            // B 0x0950183c
};

struct IsNoneUnicodeRange : public RestrictInputtingNameData
{
    IsNoneUnicodeRange() {}                            // ORIG W C1 0x08ac3240（调基类 C1）
    // ORIG W _ZNK23restrict_inputting_name18IsNoneUnicodeRangeclEw 0x08ac321c：
    // return !RestrictInputtingNameData::isInRange(ch);
    bool operator()(wchar_t ch) const;
};

bool isWideStrInUnicodeRange(const wchar_t* str);      // ORIG T 0x08ac2de4
bool isUtf8StrInUnicodeRange(const char* str);         // ORIG T 0x08ac2d6c

}  // namespace restrict_inputting_name

// ORIG T 0x0820582b _Z20checkValidCharacNamePKc（报告缺失，objdump 全文转写）：
// !(hasSpecialCharacter) && !(hasPreventString) && hasPreventStringName → true。
bool checkValidCharacName(const char* name);

#endif  // GAME_RESTRICTNAMEDEPS_H_
