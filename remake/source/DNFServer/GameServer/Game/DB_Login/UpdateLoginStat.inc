// [b28d-solo-DB_Login] verdict=AE（P211 档③同型化：PktView m_accId→unsigned int、
// m_gcNo→int 使 RHS 直装 %edx 无 eax 中转，ndiff 4→0；P259 消 stream 具名局部）。
int DB_Login::UpdateLoginStat(int userDbIdx, SIG_LOGIN_DATA* sig)
{
    // [b28d-solo-DB_Login] P259：ORIG 无 stream 具名局部（Acquire 返回值直接作
    // CStreamGuard ctor 第 2 实参；guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x3c5d), true);
    guard.operator*()->operator<<(0x2b);                      // 包 id（SIG_UPDATE_LOGIN 上报）
    guard.operator*()->operator<<(userDbIdx);
    // [b28d-solo-DB_Login] P82/P211 档③：ORIG 三条 store 均 RHS-first+disp 直写
    // （目标与 RHS 异型）⇒ 具名视图逐字段访问（裸 cast+偏移恒 LHS-first+lea）。
    struct __attribute__((packed)) PktView
    {
        unsigned int m_accId;        // +0x00（与 RHS sig->m_accId 同型 ⇒ RHS 直装 %edx 无中转）
        int          m_gcNo;         // +0x04（与 RHS sig->m_gcNo 同型 ⇒ 同上）
        unsigned int m_serverGroup;  // +0x08（目标 unsigned vs RHS int）
        char         m_ip[0x14];     // +0x0c
    };
    PktView* pkt = reinterpret_cast<PktView*>(
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGIN>());
    pkt->m_accId = sig->m_accId;
    pkt->m_serverGroup = (int)sig->m_channelRestrictField;   // +0x3a38 serverGroup(short→int)
    pkt->m_gcNo = sig->m_gcNo;                              // +0x9c
    strncpy(pkt->m_ip, SGBC(sig, OFF_LOGIN_IP), 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}
