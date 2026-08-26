#ifndef GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_
#define GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_

class Stream;

// df_game_r 还原 —— DB_UpdateChuseokPackagePurchase（2010 秋夕包购买计数
// DB 请求，DBThread 派发）。参考
// docs/class_func_reports/DB_UpdateChuseokPackagePurchase/。
// makeRequest ORIG 0x0843c8b6（_ZN31DB_UpdateChuseokPackagePurchase11makeRequestEj，
// unsigned int）；dispatch ORIG 0x0843c7bc。
class DB_UpdateChuseokPackagePurchase
{
public:
    // ORIG 0x0843c8b6：向 DB 队列发一条请求（表头 0x213 + characNo(-1) + m_id）。
    static void makeRequest(unsigned int mId);
    // ORIG 0x0843c7bc：DBThread 侧读流取 m_id 后对硬编码表
    // event_1009_package_count 执行 "upDate ... set purchase_cnt =
    // purchase_cnt+1 where m_id=%s"（无行则 insert）。
    static int dispatch(int param1, int param2, Stream* stream);
};

#endif  // GAME_DB_UPDATECHUSEOKPACKAGEPURCHASE_H_
