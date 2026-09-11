// ORIG T 0x08585fd6：流（"OnlinePreliminaryData.cpp",0x21）：<<0x1e7、<<uid；
// SIG_ONLINE_PRELIMINARY_MEMBER(0x14)：+0=u16 ServerGroup、+4=curCharacNo、
// +8=rating(位)、+0xc=win、+0x10=lose；put(DB_Q)；返回 1。
// [b28d-solo-OnlinePreliminaryTeam] 逐条对齐 ORIG：GetInBuffer 经 `guard->`
// （operator-> ptEv + GetInBuffer 两 call）、characNo 同型直存（P211 档③：
// SIG 字段 unsigned int ↔ getCurCharacNo unsigned int，ORIG 0x08586085 无
// eax→edx 中转）。残余 --no-compile 下仅链接期 call 重定位未解析 + 数据地址
// （s_stream_pool/s_msgq_mgr），relink 后转 AE，指令 85=85 全等。
bool COnlinePreliminaryMember::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp", 0x21),
        true);
    guard.operator*()->operator<<(0x1e7);
    guard.operator*()->operator<<(static_cast<int>(pUser->GetUID()));

    SIG_ONLINE_PRELIMINARY_MEMBER* p =
        guard->GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER>();
    p->m_serverGroup = static_cast<unsigned short>(pUser->GetServerGroup());
    p->m_characNo = pUser->getCurCharacNo();
    p->m_rating = m_rating;   // ORIG movl 4B 直拷
    p->m_winCount = m_winCount;
    p->m_loseCount = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}