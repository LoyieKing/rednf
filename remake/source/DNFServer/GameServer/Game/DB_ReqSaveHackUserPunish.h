#ifndef GAME_DB_REQSAVEHACKUSERPUNISH_H_
#define GAME_DB_REQSAVEHACKUSERPUNISH_H_

// ============================================================================
// df_game_r 还原 —— DB_ReqSaveHackUserPunish（黑客惩罚入库请求）
// ORIG makeRequest T 0x08425494（_ZN24DB_ReqSaveHackUserPunish11makeRequestEjjPKc，
// 报告 docs/class_func_reports/DB_ReqSaveHackUserPunish/makeRequest.md）：
//   StreamPool::Acquire("DBThread.cpp",0x6304) → guard << 0xb5、<< -1；
//   SIG_SAVE_HACK_USER_PUNISH：+0 accId / +4 punish / +8 ip[16]
//     （param_3==NULL 时 memcpy "000.000.000.000"，否则 strncpy 0x10）；
//   MsgQueueMgr::put(QUEUE_IDX=2)。
// ============================================================================
// SIG_SAVE_HACK_USER_PUNISH 权威定义在 SigTypes.h（已按本报告偏移具名化，
// sizeof 0x18）。
#include "SigTypes.h"

class DB_ReqSaveHackUserPunish
{
public:
    static void makeRequest(unsigned int accId, unsigned int punish,
                            char const* ip);
};

#endif  // GAME_DB_REQSAVEHACKUSERPUNISH_H_
