// MobileRestrict —— Taiwan 手机认证限制类别名查询
// ORIG T 0x0816d463 _ZN14MobileRestrict12enumToStringENS_4EnumE（size 0xf）：
// return enumStringMobiles[param_1]（char const* 表，步长 4）。
// 报告：docs/class_func_reports/MobileRestrict/enumToString.md
// 枚举成员值未从证据还原（ORIG mangled 仅确认嵌套 Enum 类型，int 参数）。
#ifndef GAME_MOBILERESTRICT_H_
#define GAME_MOBILERESTRICT_H_

class MobileRestrict
{
public:
    enum Enum
    {
        MOBILE_RESTRICT_UNKNOWN = 0
    };

    static char const* enumToString(Enum e);   // ORIG T @0x0816d463

private:
    // ORIG d @0x093700c0（L local，0x8c=35 项 char const*，索引即 Enum 值）：
    // {RestrictBegin, DeleteItem, DropGold, UseNpc, UseMail, UseAuction, UseTrade,
    //  UseUpgrade, UseEnchant, UseCompound, UseDisjoint, UsePrivateStore, UseStackable,
    //  UseBindSphere, UseSeal, UseRandomOptionChange, UseRandomOptionReGeneration,
    //  UseCeraShop, UseRandomBox, UseExportJob, UseDisjointAvatar, UseEmblemCompound,
    //  RecoverStamina, DeleteCharacter, AccountCargo, AccountUpgrade, AccountMoveGold,
    //  AccountMoveItem, GuildCreate, GuildLevelUp, GuildSkillUp, GuildBreak,
    //  CreateCharacter, LoginChannel, RestrictEnd}
    static char const* enumStringMobiles[35];
};

#endif  // GAME_MOBILERESTRICT_H_
