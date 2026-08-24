#ifndef GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_
#define GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_

class Stream;

// df_game_r 还原 —— DB_InsertAccountFirstLoginDailyFlag（账号每日首次登录标记 DB
// 请求，DBThread 派发）。参考 docs/class_func_reports/DB_InsertAccountFirstLoginDailyFlag/。
// makeRequest ORIG 0x0843ea64（_ZN35DB_InsertAccountFirstLoginDailyFlag11makeRequestEijji，
// int,unsigned int,unsigned int,int）；dispatch ORIG 0x0843e8be。
class DB_InsertAccountFirstLoginDailyFlag
{
public:
    // ORIG 0x0843ea64：向 DB 队列发一条请求（表头 0x342 + 4 参数）。
    static void makeRequest(int p1, unsigned int p2, unsigned int p3, int p4);
    // ORIG 0x0843e8be：DBThread 侧读流并执行 UPDATE/INSERT。
    static int dispatch(int param1, int serverGroup, Stream* stream);
};

#endif  // GAME_DB_INSERTACCOUNTFIRSTLOGINDAILYFLAG_H_
