#ifndef DNF_GAME_APSYSTEM_DB_DISPATCHERS_H_
#define DNF_GAME_APSYSTEM_DB_DISPATCHERS_H_

// ============================================================================
// df_game_r 还原 —— APSystem DB 签名分发器族（Wave5「仅ORIG」批）。
// 唯一声明点；实现 TU = APSystemOnlyOrig.cpp。
// 注意：DB_UpdateActionPoint 的 static makeRequest 声明仍留在
// DB_AccountCargoSync.h（既有归属），该类本体以 DB_AccountCargoSync.h 为准。
//
// 报告目录：docs/class_func_reports/APSystem__{DB_LoadActionPoint,
//   DB_LoadRewardMedal, DB_UpdateActionPoint, DB_UpdateRewardMedal}/
// ============================================================================

#include "DBDispatcher.h"

#include "MoreTypes.h"   // _SIG_LOAD_REWARD_MEDAL/_SIG_UPDATE_REWARD_MEDAL 完整布局
                         //（内含 APSystemTypes.h：_SIG_LOAD_ACTION_POINT/_ActionPoint）

class Stream;


namespace APSystem
{
// ---- DB_LoadActionPoint（README：4 函数 + C1/C2/D0/D1/D2 全部 W）----
class DB_LoadActionPoint : public DBDispatcher
{
public:
    DB_LoadActionPoint();     // ORIG W 0x0844dd7a C1/C2 同体（设 vptr 0x8b5f4a0）
    virtual ~DB_LoadActionPoint();   // D1 0x0812544c / D0 0x0812547c

    virtual bool dispatch(int a, int b, Stream* stream);                 // ORIG T 0x08123b10
    bool Select(_SIG_LOAD_ACTION_POINT& sig);                           // ORIG T 0x08123b9e
    bool ParseActionClearState(char* text, _ActionPoint* table,
                               unsigned int count);                     // ORIG T 0x08123f50
    bool SendResult(int a, int b, _SIG_LOAD_ACTION_POINT& sig, bool ok); // ORIG T 0x08124156

private:
    // 对象尺寸 4：仅 vptr。
};

// ---- DB_LoadRewardMedal ----
class DB_LoadRewardMedal : public DBDispatcher
{
public:
    DB_LoadRewardMedal();     // ORIG W 0x0844ddb2 C1/C2 同体（vptr 0x8b5f470）
    virtual ~DB_LoadRewardMedal();   // D1 0x081254e8 / D0 0x08125518

    void makeRequest(int uid, unsigned int characNo);                   // ORIG T 0x0812487a 空体
    virtual bool dispatch(int a, int b, Stream* stream);                 // ORIG T 0x08124882
    bool Select(_SIG_LOAD_REWARD_MEDAL& sig);                           // ORIG T 0x08124910
    bool SendResult(int a, int b, _SIG_LOAD_REWARD_MEDAL& sig);         // ORIG T 0x08124b8e

private:
};

// ---- DB_UpdateRewardMedal ----
class DB_UpdateRewardMedal : public DBDispatcher
{
public:
    DB_UpdateRewardMedal();   // ORIG W 0x0844ddce C1/C2 同体（vptr 0x8b5f458）
    virtual ~DB_UpdateRewardMedal();   // D1 0x08125536 / D0 0x08125566

    static void makeRequest(int uid, const _SIG_LOAD_REWARD_MEDAL& sig);  // ORIG T 0x08124c70（0x08124e81 调用点无 this，静态实证；InterWave1）
    virtual bool dispatch(int a, int b, Stream* stream);                 // ORIG T 0x08124c78
    bool Update(_SIG_UPDATE_REWARD_MEDAL& sig);                         // ORIG T 0x08124cb2
};

}  // namespace APSystem

#endif  // DNF_GAME_APSYSTEM_DB_DISPATCHERS_H_
