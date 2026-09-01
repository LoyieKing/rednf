#ifndef GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_
#define GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_

#include "DBDispatcher.h"

class Stream;

// df_game_r 还原 —— DB_UpdateChuseokPackagePurchase（2010 秋夕包购买计数
// DB 请求，DBThread 派发）。参考
// docs/class_func_reports/DB_UpdateChuseokPackagePurchase/。
// makeRequest ORIG 0x0843c8b6（_ZN31DB_UpdateChuseokPackagePurchase11makeRequestEj，
// unsigned int）；dispatch ORIG 0x0843c7bc。
// ORIG 反汇编取证：makeRequest 从 0x8(%ebp) 读 m_id（无 this ⇒ static）；
// dispatch 从 0x14(%ebp) 读 stream（this@0x8 / p1@0xc / p2@0x10 ⇒ 非 static
// 成员函数）。W8LT_DBTask.cpp m_dispatchers[524] 注册（ORIG C1 0x844f502 仅
// base ctor+vptr；D1 0x845c328 / D0 0x845c358；vtable
// _ZTV31DB_UpdateChuseokPackagePurchase）⇒ DBDispatcher 派生 + 虚重写。
class DB_UpdateChuseokPackagePurchase : public DBDispatcher
{
public:
    // ORIG 0x0843c8b6：向 DB 队列发一条请求（表头 0x213 + characNo(-1) + m_id）。
    static void makeRequest(unsigned int mId);
    // ORIG 0x0843c7bc：DBThread 侧读流取 m_id 后对硬编码表
    // event_1009_package_count 执行 "upDate ... set purchase_cnt =
    // purchase_cnt+1 where m_id=%s"（无行则 insert）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_
