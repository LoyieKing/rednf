// ORIG T 0x08587ee4：m_hasWinFlag!=1 直通返回 1；
// 流（"OnlinePreliminaryTeam.cpp",0x38）：<<0x1e9、<<-1，
// SIG_ONLINE_PRELIMINARY_TEAM(buf 0x20)：+0=teamId(+0x4)、+0x14=rating(+0x8
// 位模式)、+0x18=win、+0x1c=lose；put(DB_Q)；清 m_hasWinFlag。
// [b22判词·N分支lowering] ORIG 头判定 `movzbl; xor $0x1,%eax; test %al,%al; je
// body`（把 !flag 物化 xor 后再测，je 跳 body、fall-through 返 1），本 gcc 对
// `m_hasWinFlag != 1` 合并降为 `cmp $0x1,%al; je body`（少一条独立 xor，块布局/
// 跳转方向/return 路径与 ORIG 逐条一致）。语义等价（flag∈{0,1}，仅 ==1 落库），
// xor-vs-cmp 系编译器分支 lowering 版本差，源码不可复现。其余残余为链接期
// call 重定位未解析 + 数据地址（--no-compile 读未 relink .o），relink 后转 AE。
bool COnlinePreliminaryTeam::_saveTeam()
{
    if (!m_hasWinFlag)
        return true;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x38),
        true);
    guard.operator*()->operator<<(0x1e9);
    guard.operator*()->operator<<(-1);

    // 具名字段写入（布局证据 SigTypes_D.h::SIG_ONLINE_PRELIMINARY_TEAM）：
    // +0 teamId / +0x14 rating 位 / +0x18 win / +0x1c lose，ORIG 逐条 movl。
    SIG_ONLINE_PRELIMINARY_TEAM* t =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM>();
    t->m_teamId = m_field04;
    t->m_rating = m_rating08;
    t->m_winCount = m_winCount;
    t->m_loseCount = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_hasWinFlag = 0;
    return true;
}