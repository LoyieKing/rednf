// CharacterRestrict —— Taiwan 角色创建限制类别名查询
// ORIG T 0x0816d472 _ZN17CharacterRestrict12enumToStringENS_4EnumE（size 0xf）：
// return enumStringCharacter[param_1]（char const* 表，步长 4，与 MobileRestrict 同构）。
// 报告：docs/class_func_reports/CharacterRestrict/
// 枚举成员值未从证据还原（ORIG mangled 仅确认嵌套 Enum 类型，int 参数）。
#ifndef DNF_GAME_CHARACTERRESTRICT_H_
#define DNF_GAME_CHARACTERRESTRICT_H_

class CharacterRestrict
{
public:
    enum Enum
    {
        CHARACTER_RESTRICT_UNKNOWN = 0
    };

    static char const* enumToString(Enum e);   // ORIG T @0x0816d472

private:
    // ORIG d @0x0937014c（L local，0x10=4 项 char const*，索引即 Enum 值）raw 实证序：
    // {RestrictBegin, RestrictOnDay, RestrictIP, RestrictEnd}
    // 【修正 b28/R18obj2】原注释写 {RestrictBegin, RestrictEnd, RestrictOnDay, RestrictIP}
    // 的 [1..3] 与 ORIG 轮换错位；定义处（SecurityRestrictPolicy.cpp）已按 ORIG 顺序修正。
    static char const* enumStringCharacters[4];
};

#endif  // DNF_GAME_CHARACTERRESTRICT_H_
