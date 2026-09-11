// ORIG 0x0843915c sz 0x17f（_ZN26DB_LoadBreakAwayRewardData11makeRequestEijjR18STBreakAwayRewards）：
// Acquire("DBThread.cpp",0x86d3) → CStreamGuard(stream,true)
//   → <<0x142(int) → <<param_1(int) → <<param_2(uint) → <<param_3(uint)
//   → <<rewards+0x00(uint) → <<rewards+0x04(int) → <<rewards+0x08(int)
//   → <<rewards+0x10(int) → <<rewards+0x0c(uint)（ORIG 字段写序 +0x10 先于 +0xc）
//   → put(DB_Q(2))。
void DB_LoadBreakAwayRewardData::makeRequest(
    int userIdx, unsigned int param2, unsigned int param3,
    STBreakAwayRewards& rewards)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x86d3), true);
    guard.operator*()->operator<<((int)0x142);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((unsigned int)param3);
    // ORIG 0x08439204 `mov (%eax),%ebx` + call _ZN12CStreamGuardlsEj
    // （unsigned int 重载，080e2292）——字段必须走 uint 重载；单条装载要求
    // 字段类型为 unsigned int（CUser.h 现声明 int，已上报 Main 代落）。
    guard.operator*()->operator<<((unsigned int)rewards.m_rewardItemId);
    guard.operator*()->operator<<((int)rewards.m_rewardAItemCnt);
    guard.operator*()->operator<<((int)rewards.m_luckPoint);
    guard.operator*()->operator<<((int)rewards.m_itemOrder);
    guard.operator*()->operator<<((unsigned int)rewards.m_fieldc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}