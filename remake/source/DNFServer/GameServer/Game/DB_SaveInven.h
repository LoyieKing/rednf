#ifndef GAME_DB_SAVEINVEN_H_
#define GAME_DB_SAVEINVEN_H_

// ============================================================================
// df_game_r 还原 —— DB_SaveInven（DB 线程背包保存分发器）
// 报告：docs/class_func_reports/DB_SaveInven/（9 符号，地址见各注释）。
// 布局：仅基类 DBDispatcher 子对象，sizeof = 4（vptr）。
//   vtable _ZTV12DB_SaveInven @0x8c5fad8 [dispatch][~D1][~D0]，
//   typeinfo @0x8c62e40 [nm]。
// 实现唯一 owner：DB_SaveInven.cpp。
// ============================================================================

#include "DBDispatcher.h"

class MySQL;
class Stream;
struct SIG_SAVE_INVENTORY;

class DB_SaveInven : public DBDispatcher
{
public:
    DB_SaveInven();                                    // C1/C2 T @0x08416a42
    ~DB_SaveInven();                                   // D1 T @0x08416a5e / D0 T @0x08416a8e
                                                       // （D2 由 GCC 随非内联析构发射）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // T @0x08416aac

    bool SaveInven(SIG_SAVE_INVENTORY* sig);           // T @0x08416cfa
    int SendResult(int userIdx, int slot, SIG_SAVE_INVENTORY* sig);  // T @0x08417204
    bool _saveCharacInvenExpand(SIG_SAVE_INVENTORY* sig) const;     // T @0x08416b38
};

#endif  // GAME_DB_SAVEINVEN_H_
