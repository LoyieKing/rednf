void CUser::requestCubePremiumInfoIfContracted()
{
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x086467ee：`test %al,%al; je <T>` 无 xor ⇒ 正向 bool 直落形
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c)) {
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("user.cpp", 0x475), true);
    guard.operator*()->operator<<(0x332);
    guard.operator*()->operator<<(static_cast<int>(GetUID()));
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO* sig =
        guard->GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO>();
    sig->m_characNo = static_cast<unsigned int>(getCurCharacR()->m_characNo);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
