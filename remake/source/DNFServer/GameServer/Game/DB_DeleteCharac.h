#ifndef GAME_DB_DELETECHARAC_H_
#define GAME_DB_DELETECHARAC_H_

// ============================================================================
// df_game_r 还原 —— DB_DeleteCharac（SIG_DELETE_CHARAC 删除角色 DB 分发器，
// DB 线程侧）。ORIG 10 个符号报告见 docs/class_func_reports/DB_DeleteCharac/：
//   ctor 0x08402bce（调 DBDispatcher::DBDispatcher 后设 vptr）
//   ~DB_DeleteCharac 0x08402bea（D1，设 vptr→基类 dtor→恒假 delete 分支）
//   ~DB_DeleteCharac 0x08402c1a（D0 deleting）
//   dispatch 0x08402c38（virtual bool，覆盖 DBDispatcher::dispatch）
//   DeleteGuildInfo 0x08402f4c / DeleteEventCharacFlag 0x084032aa /
//   CheckEventCharacter40 0x08403324 / DeleteCharac 0x0840340c /
//   SendResult 0x084034fe / DeleteCharacOption 0x084035e0
// 对象尺寸 4（仅 vptr），基类 DBDispatcher 见同目录头。
// ============================================================================

#include "SigTypes.h"      // SIG_DELETE_CHARAC
#include "DBDispatcher.h"
class Stream;

class DB_DeleteCharac : public DBDispatcher
{
public:
    DB_DeleteCharac();                          // C1 0x08402bce
    virtual ~DB_DeleteCharac();                 // D1 0x08402bea（D0 0x08402c1a）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);
                                                // ORIG T 0x08402c38

    bool DeleteGuildInfo(SIG_DELETE_CHARAC* sig);        // 0x08402f4c
    bool DeleteEventCharacFlag(SIG_DELETE_CHARAC* sig);  // 0x084032aa
    bool CheckEventCharacter40(SIG_DELETE_CHARAC* sig);  // 0x08403324
    bool DeleteCharac(SIG_DELETE_CHARAC* sig);           // 0x0840340c
    bool SendResult(int userIdx, int slot,
                    SIG_DELETE_CHARAC* sig);             // 0x084034fe
    bool DeleteCharacOption(SIG_DELETE_CHARAC* sig);     // 0x084035e0
};

#endif  // GAME_DB_DELETECHARAC_H_
