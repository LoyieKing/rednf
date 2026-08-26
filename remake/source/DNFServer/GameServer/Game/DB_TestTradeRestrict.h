#ifndef GAME_DB_TESTTRADERESTRICT_H_
#define GAME_DB_TESTTRADERESTRICT_H_

// df_game_r 还原 —— DB_TestTradeRestrict（交易限制测试 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_TestTradeRestrict/makeRequest.md，
// ORIG 0x0844a132 sz 0xd7。
class DB_TestTradeRestrict
{
public:
    // ORIG T 0x0844a132：mangled _ZN20DB_TestTradeRestrict11makeRequestEj
    // 流：表头 0x31c + -1，正文 accId(u32)。Acquire 行号常量 0xba73。
    static void makeRequest(unsigned int accId);
};

#endif  // GAME_DB_TESTTRADERESTRICT_H_
