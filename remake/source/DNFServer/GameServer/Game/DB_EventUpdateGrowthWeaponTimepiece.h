#ifndef GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_
#define GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_

// df_game_r 还原 —— DB_EventUpdateGrowthWeaponTimepiece（成长武器怀表更新 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_EventUpdateGrowthWeaponTimepiece/makeRequest.md，
// ORIG 0x084497ea sz 0x113。
class DB_EventUpdateGrowthWeaponTimepiece
{
public:
    // ORIG T 0x084497ea：mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
    // 流：表头 0x315 + -1，正文 accId(u32)、timepiece(short)、value(int)。
    // Acquire 行号常量 0xb941。
    static void makeRequest(unsigned int accId, short timepiece, int value);
};

#endif  // GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_
