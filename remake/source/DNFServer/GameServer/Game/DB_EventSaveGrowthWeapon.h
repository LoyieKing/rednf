#ifndef GAME_DB_EVENTSAVEGROWTHWEAPON_H_
#define GAME_DB_EVENTSAVEGROWTHWEAPON_H_

// df_game_r 还原 —— DB_EventSaveGrowthWeapon（成长武器活动礼包领取 DB 请求）。
// 唯一声明点（makeRequest 由 GrowthEquipEvent::GiftGrowthWeaponBox 调用）。
// 参考 docs/class_func_reports/DB_EventSaveGrowthWeapon/makeRequest.md，
// ORIG 0x084493e8；dispatch 0x084492fc / ctor 0x0844fd8a / dtor 0x0845ae70。
class DB_EventSaveGrowthWeapon
{
public:
    // ORIG T 0x084493e8：mangled _ZN24DB_EventSaveGrowthWeapon11makeRequestEjs
    // 流：表头 0x313 + -1，正文 accId(u32)、serverGroup(short)。
    static void makeRequest(unsigned int accId, short serverGroup);
};

#endif  // GAME_DB_EVENTSAVEGROWTHWEAPON_H_
