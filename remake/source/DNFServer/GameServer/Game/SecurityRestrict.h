// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG +
// docs/class_func_reports/SecurityRestrict/enumToString.md
// SecurityRestrict：ORIG T enumToString(SecurityRestrict::Enum) @0x0816d481，
// 直接查静态字符串表 enumStringSecurity（d @0x0937015c，char* 数组按 4 字节索引）。
// 相关 SecurityRestrictPolicy（begin/end/isApply/printRestrict 等）为独立类，不在本头。
#ifndef DNF_GAME_SECURITYRESTRICT_H_
#define DNF_GAME_SECURITYRESTRICT_H_

class SecurityRestrict
{
public:
    // 【推断】ORIG 枚举成员值未从证据还原，仅确认存在嵌套 Enum 类型
    // （mangled _ZN16SecurityRestrict12enumToStringENS_4EnumE）。
    enum Enum
    {
        SECURITY_RESTRICT_UNKNOWN = 0
    };

    static char const* enumToString(Enum e);   // ORIG T @0x0816d481

private:
    // ORIG d @0x0937015c：char const* 表，索引即 Enum 值。
    static char const* enumStringSecurity[];   // 【推断】元素个数未证实
};

#endif  // DNF_GAME_SECURITYRESTRICT_H_
