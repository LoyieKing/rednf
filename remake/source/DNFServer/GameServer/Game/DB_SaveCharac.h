#ifndef GAME_DB_SAVECHARAC_H_
#define GAME_DB_SAVECHARAC_H_

// ============================================================================
// df_game_r 还原 —— DB_SaveCharac（DB 线程角色保存分发器）
// 报告：docs/class_func_reports/DB_SaveCharac/（9 符号，地址见各注释）。
// 布局：仅基类 DBDispatcher 子对象，sizeof = 4（vptr）。
//   vtable _ZTV13DB_SaveCharac @0x8c5faf0 [dispatch][~D1][~D0]，
//   typeinfo @0x8c62e5c [nm]。
// 实现唯一 owner：DB_SaveCharac.cpp。
// ============================================================================

#include "DBDispatcher.h"

class MySQL;
class Stream;
struct SIG_SAVE_CHARAC;

class DB_SaveCharac : public DBDispatcher
{
public:
    DB_SaveCharac() __attribute__((nothrow));           // C1/C2 T @0x08415c2e；try 区无 pad ⇒ nothrow
    ~DB_SaveCharac();                                   // D1 T @0x08415c4a / D0 T @0x08415c7a
                                                        // （D2 由 GCC 随非内联析构发射）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // T @0x08415c98

    int SendResult(int userIdx, int slot, SIG_SAVE_CHARAC* sig);   // T @0x08416904
    bool _SaveCharacInfo(SIG_SAVE_CHARAC* sig);         // T @0x08415eda
    bool _SaveCharacStat(SIG_SAVE_CHARAC* sig);         // T @0x084163f2
    bool _SaveCharacManageInfo(SIG_SAVE_CHARAC* sig);   // T @0x0841690e
    bool _SaveContractOfCubePremiumInfo(SIG_SAVE_CHARAC const& sig);  // T @0x08415dbe
};

#endif  // GAME_DB_SAVECHARAC_H_
