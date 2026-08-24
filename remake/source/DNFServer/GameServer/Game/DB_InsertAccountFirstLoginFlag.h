#ifndef GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_
#define GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_

class Stream;

// df_game_r 还原 —— DB_InsertAccountFirstLoginFlag（账号首次登录标记 DB 请求，
// DBThread 派发）。参考 docs/class_func_reports/DB_InsertAccountFirstLoginFlag/。
// makeRequest ORIG 0x0843e79a（_ZN30DB_InsertAccountFirstLoginFlag11makeRequestEijiji，
// int,unsigned int,int,unsigned int,int）；dispatch ORIG 0x0843e636。
class DB_InsertAccountFirstLoginFlag
{
public:
    // ORIG 0x0843e79a：向 DB 队列发一条请求（表头 0x25e + 5 参数）。
    static void makeRequest(int p1, unsigned int p2, int p3,
                            unsigned int p4, int p5);
    // ORIG 0x0843e636：DBThread 侧读流并执行 INSERT。
    static int dispatch(int param1, int serverGroup, Stream* stream);
};

#endif  // GAME_DB_INSERTACCOUNTFIRSTLOGINFLAG_H_
