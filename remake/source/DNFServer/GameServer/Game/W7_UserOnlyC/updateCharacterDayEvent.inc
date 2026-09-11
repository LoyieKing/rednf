void CUserEventUtil::updateCharacterDayEvent(CUser* user, int day)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x9af5), true);
    // ORIG 每次写入均重调 operator*（P48），GetInBuffer 经 operator->。
    (**guard) << 0x16e;
    (**guard) << static_cast<int>(user->GetUID());

    SIG_UPDATE_CHARACTER_DAY_EVENT* sig =
        guard->GetInBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>();
    sig->m_accId = user->get_acc_id();
    sig->m_day = static_cast<short>(day);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}