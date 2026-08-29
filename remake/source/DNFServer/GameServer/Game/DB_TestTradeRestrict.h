#ifndef GAME_DB_TESTTRADERESTRICT_H_
#define GAME_DB_TESTTRADERESTRICT_H_

// df_game_r 还原 —— DB_TestTradeRestrict（交易限制测试 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_TestTradeRestrict/{makeRequest,dispatch}.md，
// makeRequest ORIG 0x0844a132 sz 0xd2；dispatch ORIG 0x0844a0be sz 0x73。
// 继承 DBDispatcher：ORIG vtable _ZTV20DB_TestTradeRestrict @0x08c5e068
// [offtop][typeinfo][dispatch=0x0844a0be][~D1=0x0845ac4e][~D0=0x0845ac7e]；
// C1/C2 W @0x0844fe6a（对象尺寸 4 = 仅 vptr，W8LT_DBTask.cpp 局部类提供）。
#include "DBDispatcher.h"

class DB_TestTradeRestrict : public DBDispatcher
{
public:
    DB_TestTradeRestrict() {}

    // ORIG T 0x0844a132：mangled _ZN20DB_TestTradeRestrict11makeRequestEj
    // 流：表头 0x31c + -1，正文 accId(u32)。Acquire 行号常量 0xba73。
    static void makeRequest(unsigned int accId);

    // ORIG T 0x0844a0be：mangled _ZN20DB_TestTradeRestrict8dispatchEiiP6Stream
    // GetDBHandle(DB_HANDLE_3,0) → >>charac_no(u32) → UPDATE trade_restrict=1 → 恒返回 true。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};

#endif  // GAME_DB_TESTTRADERESTRICT_H_
