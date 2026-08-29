#ifndef GAME_DB_EVENTGROWTHWEAPONRESET_H_
#define GAME_DB_EVENTGROWTHWEAPONRESET_H_

// df_game_r 还原 —— DB_EventGrowthWeaponReset（成长武器重置 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_EventGrowthWeaponReset/{makeRequest,dispatch}.md，
// makeRequest ORIG 0x08449980 sz 0xd7；dispatch ORIG 0x084498fe sz 0x81。
// 继承 DBDispatcher：ORIG vtable _ZTV25DB_EventGrowthWeaponReset @0x08c5e0c8
// [offtop][typeinfo][dispatch=0x084498fe][~D1=0x0845ad86][~D0=0x0845adb6]；
// C1/C2 W @0x0844fdde（本类对象尺寸 4 = 仅 vptr，W8LT_DBTask.cpp 局部类提供）。
#include "DBDispatcher.h"

class DB_EventGrowthWeaponReset : public DBDispatcher
{
public:
    DB_EventGrowthWeaponReset() {}

    // ORIG T 0x08449980：mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
    // 流：表头 0x316 + -1，正文 accId(u32)。Acquire 行号常量 0xb95e。
    static void makeRequest(unsigned int accId);

    // ORIG T 0x084498fe：mangled _ZN25DB_EventGrowthWeaponReset8dispatchEiiP6Stream
    // GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) → DELETE 语句 → exec 成功返回 true。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_EVENTGROWTHWEAPONRESET_H_
