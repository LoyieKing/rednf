#ifndef GAME_DB_UPDATEPURCHASECASHITEMBONUS_H_
#define GAME_DB_UPDATEPURCHASECASHITEMBONUS_H_

class Stream;

// df_game_r 还原 —— DB_UpdatePurchaseCashItemBonus（商城消费奖励购买计数
// DB 请求，DBThread 派发）。参考
// docs/class_func_reports/DB_UpdatePurchaseCashItemBonus/。
// makeRequest ORIG 0x0843e560（_ZN30DB_UpdatePurchaseCashItemBonus11makeRequestEij，
// int,unsigned int）；dispatch ORIG 0x0843e252。
class DB_UpdatePurchaseCashItemBonus
{
public:
    // ORIG 0x0843e560：向 DB 队列发一条请求（表头 0x23c + m_id + acc_id）。
    static void makeRequest(int p1, unsigned int p2);
    // ORIG 0x0843e252：DBThread 侧读流并执行 update/insert/select，
    // 结果经 DISPATCH_Q 回发 0x23c 包（SIG_PURCHASE_CASHITEM_BONUS）。
    static int dispatch(int param1, int param2, Stream* stream);
};

#endif  // GAME_DB_UPDATEPURCHASECASHITEMBONUS_H_
