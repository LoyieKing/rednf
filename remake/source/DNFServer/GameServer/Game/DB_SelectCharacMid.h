#ifndef GAME_DB_SELECTCHARACMID_H_
#define GAME_DB_SELECTCHARACMID_H_

// ============================================================================
// df_game_r 还原 —— DB_SelectCharacMid（DBThread 查询：按角色名取 mid/审计计数）
// ORIG makeRequest T 0x0844676c（_ZN18DB_SelectCharacMid11makeRequestEijPKcjjj，
// 报告 docs/class_func_reports/DB_SelectCharacMid/makeRequest.md）：
//   StreamPool::Acquire("DBThread.cpp",0xae42) → guard << 0x2b9、<< param_1；
//   SIG_CHECK_CHAR_MID：+0 param_2 / +4 param_4 / +8 param_5 / +0xc param_6 /
//   +0x10 strncpy(param_3, 0x1e)；MsgQueueMgr::put(QUEUE_IDX=2)。
// ============================================================================

#include <stddef.h>
// SIG_CHECK_CHAR_MID 权威定义在 SigTypes.h（已按本报告偏移具名化，sizeof 0x30）。
#include "SigTypes.h"
class DB_SelectCharacMid
{
public:
    static void makeRequest(int serverGroup, unsigned int field2,
                            char const* name, unsigned int field4,
                            unsigned int field5, unsigned int field6);
};

#endif  // GAME_DB_SELECTCHARACMID_H_
