#ifndef GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_
#define GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_

// df_game_r 还原 —— DB_EventUpdateGrowthWeaponTimepiece（成长武器时计更新 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_EventUpdateGrowthWeaponTimepiece/{makeRequest,dispatch}.md，
// makeRequest ORIG 0x084497ea sz 0x105；dispatch ORIG 0x084496f4 sz 0xf5。
// 继承 DBDispatcher：ORIG vtable _ZTV35DB_EventUpdateGrowthWeaponTimepiece @0x08c5e0e0
// [offtop][typeinfo][dispatch=0x084496f4][~D1=0x0845add4][~D0=0x0845ae04]；
// C1/C2 W @0x0844fdc2（对象尺寸 4 = 仅 vptr，W8LT_DBTask.cpp 局部类提供）。
#include "DBDispatcher.h"

class DB_EventUpdateGrowthWeaponTimepiece : public DBDispatcher
{
public:
    DB_EventUpdateGrowthWeaponTimepiece() {}

    // ORIG T 0x084497ea：mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
    // 流：表头 0x315 + -1，正文 accId(u32)/timepiece(short)/value(int)。Acquire 行号 0xb941。
    static void makeRequest(unsigned int accId, short timepiece, int value);

    // ORIG T 0x084496f4：mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece8dispatchEiiP6Stream
    // GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32)/>>timepiece(short)/>>infinityweapon_id(int) →
    // UPDATE（id!=0 时带 reward_occ_date=now()）→ exec 成功返回 true。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_EVENTUPDATEGROWTHWEAPONTIMEPIECE_H_
