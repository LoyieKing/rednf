#ifndef GAME_DB_EVENTGROWTHWEAPONRESET_H_
#define GAME_DB_EVENTGROWTHWEAPONRESET_H_

// df_game_r 还原 —— DB_EventGrowthWeaponReset（成长武器重置 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_EventGrowthWeaponReset/makeRequest.md，
// ORIG 0x08449980 sz 0xd7。
class DB_EventGrowthWeaponReset
{
public:
    // ORIG T 0x08449980：mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
    // 流：表头 0x316 + -1，正文 accId(u32)。Acquire 行号常量 0xb95e。
    static void makeRequest(unsigned int accId);
};

#endif  // GAME_DB_EVENTGROWTHWEAPONRESET_H_
