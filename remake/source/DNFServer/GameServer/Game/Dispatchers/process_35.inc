int Dispatcher_SecurityLog::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825fb38：acc_id = user==0 ? 0 : user->get_acc_id()；
    // 当 *(int*)(msg+0x15)!=0 且 *(int*)(msg+0x19)!=0 时
    // LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp", PRETTY, 0x9d0,
    //   "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
    //   *(msg+0xd), *(msg+0x11), *(msg+0x15), *(msg+0x19), *(msg+0x1d), *(msg+0x21),
    //   *(int*)(msg+0x1d) - *(int*)(msg+0x19), NumberToString(acc_id, 0))；
    // 恒返 0。
    unsigned int accId;
    if (user == 0)
    {
        accId = 0;
    }
    else
    {
        accId = user->get_acc_id();
    }
    const MSG_SECURITY_LOG_PACKET* m =
        reinterpret_cast<const MSG_SECURITY_LOG_PACKET*>(&msg);
    if (m->m_logCounts[1] != 0 && m->m_logCounts[2] != 0)  // msg+0x15 / +0x19
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x9d0,
            "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
            m->m_field0d,                                   // msg+0xd
            (int)m->m_logCounts[0],                         // msg+0x11
            (int)m->m_logCounts[1],                         // msg+0x15
            (int)m->m_logCounts[2],                         // msg+0x19
            (int)m->m_logCounts[3],                         // msg+0x1d
            (int)m->m_logCounts[4],                         // msg+0x21
            (int)m->m_logCounts[3] - (int)m->m_logCounts[2],
            NumberToString(accId, 0));
    }
    return 0;
