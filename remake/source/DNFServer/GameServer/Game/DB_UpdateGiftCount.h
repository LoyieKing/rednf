// df_game_r 还原 —— Cera 商城赠礼计数与 break-away 已用 Cera 的两个
// DB 消息类（W7 链接收敛批次）。
//
// class DB_UpdateGiftCount:
//   vtable @0x8c5f3a0 [offtop][typeinfo@0x8c6219c][dispatch][~D1][~D0]，
//   槽序与基类 DBDispatcher 一致（db_AccountCargoSync.h 同型先例）。
//   typeinfo name for DB_UpdateGiftCount @0x8c621a8。
//
// class DB_UpdateBreakAwayAccruedCera:
//   ORIG 完整虚类（vtable @0x8c5eae8 / typeinfo @0x8c61918，
//   dispatch T @0x0843865c，C1/C2 W @0x0844ed5a，D1/D2 W @0x0845ce6e），
//   本批次仅落被 CBreakAwayPreventSystem::updateUsedCera 直接引用的
//   makeRequest；生命周期/dispatch 留待 break_away 域批次补齐【推断】。
#pragma once

#include "DBDispatcher.h"

class Stream;

class DB_UpdateGiftCount : public DBDispatcher
{
public:
    // ORIG T @0x084271ec [makeRequest.md]：栈参 (uint accId, int count)，
    // 无 this 压栈（调用点 CCeraShop buy 链也以对象临时触发）→ 静态成员，
    // mangled 与普通成员一致：_ZN18DB_UpdateGiftCount11makeRequestEji。
    static void makeRequest(unsigned int accId, int count);

    DB_UpdateGiftCount();                  // ORIG W C1/C2 @0x0844e756：仅设 vptr
    virtual ~DB_UpdateGiftCount();         // D1 @0x0845eac4 / D0 @0x0845eaf4

    // ORIG T @0x08427112 [dispatch.md]：member_login 更新赠礼时间/次数。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

class DB_UpdateBreakAwayAccruedCera
{
public:
    // ORIG T @0x0843856a [DB_UpdateBreakAwayAccruedCera/makeRequest.md]：
    // 栈参 (uint param_1, uint param_2)，无 this → 静态成员；
    // _ZN29DB_UpdateBreakAwayAccruedCera11makeRequestEjj。
    static void makeRequest(unsigned int param_1, unsigned int param_2);
};
