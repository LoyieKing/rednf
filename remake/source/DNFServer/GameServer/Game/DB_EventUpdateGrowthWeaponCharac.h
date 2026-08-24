#ifndef GAME_DB_EVENTUPDATEGROWTHWEAPONCHARAC_H_
#define GAME_DB_EVENTUPDATEGROWTHWEAPONCHARAC_H_

// df_game_r 还原 —— DB_EventUpdateGrowthWeaponCharac（成长武器活动角色选择
// DB 请求，DBThread 派发）。唯一声明点（makeRequest 由 GrowthEquipEvent.cpp 调用）。
// 参考 docs/class_func_reports/DB_EventUpdateGrowthWeaponCharac/makeRequest.md，
// ORIG 0x084495c6；dispatch 0x084494e2 / ctor 0x0844fda6 / dtor 0x0845ae22。
class DB_EventUpdateGrowthWeaponCharac
{
public:
    // ORIG T 0x084495c6：mangled _ZN32DB_EventUpdateGrowthWeaponCharac11makeRequestEjjis
    // 流：表头 0x314 + -1，正文 accId(u32)、characNo(u32)、addInfo(int)、serverGroup(short)。
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            int addInfo, short serverGroup);
};

#endif  // GAME_DB_EVENTUPDATEGROWTHWEAPONCHARAC_H_
