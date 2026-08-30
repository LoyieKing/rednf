#ifndef GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_
#define GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_

#include "DBDispatcher.h"

class Stream;

// df_game_r 还原 —— DB_InsertAccountFirstLoginFlag（账号首次登录标记 DB 请求，
// DBThread 派发）。参考 docs/class_func_reports/DB_InsertAccountFirstLoginFlag/。
// makeRequest ORIG 0x0843e79a（_ZN30DB_InsertAccountFirstLoginFlag11makeRequestEijiji，
// int,unsigned int,int,unsigned int,int）；dispatch ORIG 0x0843e636。
//
// ORIG 反汇编证据：dispatch 全程从 0x14(%ebp) 读 stream（成员函数 this@0x8，
// 参数 @0xc/@0x10/@0x14），且由 DBThread 虚派发表（m_dispatchers[606]，
// ORIG C1 0x844f5e2 设 vptr）调用 → dispatch 为 DBDispatcher 虚重写，非 static。
class DB_InsertAccountFirstLoginFlag : public DBDispatcher
{
public:
    // ORIG 0x0843e79a：向 DB 队列发一条请求（表头 0x25e + 5 参数）。
    static void makeRequest(int p1, unsigned int p2, int p3,
                            unsigned int p4, int p5);
    // ORIG 0x0843e636：DBThread 侧读流并执行 INSERT（虚重写，vtable 首槽）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_
