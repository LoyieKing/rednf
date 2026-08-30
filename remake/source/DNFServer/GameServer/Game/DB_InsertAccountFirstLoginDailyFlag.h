#ifndef GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_
#define GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_

#include "DBDispatcher.h"

class Stream;

// df_game_r 还原 —— DB_InsertAccountFirstLoginDailyFlag（账号每日首次登录标记 DB
// 请求，DBThread 派发）。参考 docs/class_func_reports/DB_InsertAccountFirstLoginDailyFlag/。
// makeRequest ORIG 0x0843ea64（_ZN35DB_InsertAccountFirstLoginDailyFlag11makeRequestEijji，
// int,unsigned int,unsigned int,int）；dispatch ORIG 0x0843e8be。
// ORIG 反汇编：dispatch 从 0x14(%ebp) 读 stream（成员函数 this@0x8），
// DBThread 虚派发表（m_dispatchers[834]，ORIG C1 0x844ff9e）→ 虚重写。
class DB_InsertAccountFirstLoginDailyFlag : public DBDispatcher
{
public:
    // ORIG 0x0843ea64：向 DB 队列发一条请求（表头 0x342 + 4 参数）。
    static void makeRequest(int p1, unsigned int p2, unsigned int p3, int p4);
    // ORIG 0x0843e8be：DBThread 侧读流并执行 UPDATE/INSERT（虚重写）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_
