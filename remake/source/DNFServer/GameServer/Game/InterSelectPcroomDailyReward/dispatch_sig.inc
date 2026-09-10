int InterSelectPcroomDailyReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Msg* msg = reinterpret_cast<Msg*>(data);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x19a, "[Taiwan, pcroom] interMsg is null.");
        return 0x19b;
    }
    user->m_field8ec32 = msg->m_remainCount;
    register int remain = (signed char)msg->m_remainCount;  // ORIG movsbl → esi
    register char* idStr = NumberToString(msg->m_id, 0);    // ORIG ebx 缓存
    cMyTrace trace("virtual int InterSelectPcroomDailyReward::dispatch_sig(CUser*, char*, int)",
                   0x19f, 0);
    trace("[Taiwan, pcroom] m_id:%s, remain_reward_count:%d", idStr, remain);
    SendPacket(user);
    return 0;
