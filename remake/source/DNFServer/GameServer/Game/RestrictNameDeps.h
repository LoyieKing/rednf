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
    static bool isLoaded_;                           // B 0x0950183c（bool 化：ORIG C1 xor$1/je 测试形 + mov %al 直存实证）
};

struct IsNoneUnicodeRange : public RestrictInputtingNameData
{
    IsNoneUnicodeRange() {}                            // ORIG W C1 0x08ac3240（调基类 C1）
    // ORIG W _ZNK23restrict_inputting_name18IsNoneUnicodeRangeclEw 0x08ac321c：
    // return !RestrictInputtingNameData::isInRange(ch);
    bool operator()(wchar_t ch) const;

    // [b28d-solo] ORIG 4.1 对谓词按值传参时从栈槽读字节（isWideStrInUnicodeRange
    // 0x08ac2e0c `movzbl -0x11(%ebp),%edx; mov %dl,0x8(%esp)`）；4.4 对**空类**对象
    // 做标量替换改传 `%bl`（探针 /tmp/probe_sra2.cpp：空类 push %ebx+sub $0x24+mov %bl，
    // 带 1 字节成员 sub $0x28+movzbl -N(%ebp)）。加 1 字节成员后 4.4 复现槽读取；
    // sizeof 仍 1（空类 sizeof==1），基类布局/全部偏移/mangled/符号集零变化，
    // C1 体仍只调基类 C1（ORIG 亦不写该字节）。无业务语义，仅恢复 ORIG 字节传递形态。
    char m_reserved;
};

bool isWideStrInUnicodeRange(const wchar_t* str);      // ORIG T 0x08ac2de4
bool isUtf8StrInUnicodeRange(const char* str);         // ORIG T 0x08ac2d6c

}  // namespace restrict_inputting_name

// ORIG T 0x0820582b _Z20checkValidCharacNamePKc（报告缺失，objdump 全文转写）：
// !(hasSpecialCharacter) && !(hasPreventString) && hasPreventStringName → true。
bool checkValidCharacName(const char* name);

#endif  // GAME_RESTRICTNAMEDEPS_H_
