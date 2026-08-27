#ifndef GAME_DB_INSERTONTIMERETARDUSERLOG_H_
#define GAME_DB_INSERTONTIMERETARDUSERLOG_H_

// DB_InsertOnTimeRewardUserLog —— 整点奖励发放日志 DB 请求（LOGDB 落盘）。
// ORIG T makeRequest @0x08442b7a：Acquire → CStreamGuard(stream,true) →
// <<0x298(cmd) → <<uid(int) → <<accId(uint) → <<ontimeId(int) →
// MsgQueueMgr::put(s_msgq_mgr, DB_Q=2, guard)。
class DB_InsertOnTimeRewardUserLog
{
public:
    static void makeRequest(int uid, unsigned int accId, int ontimeId);
};

#endif  // GAME_DB_INSERTONTIMERETARDUSERLOG_H_
