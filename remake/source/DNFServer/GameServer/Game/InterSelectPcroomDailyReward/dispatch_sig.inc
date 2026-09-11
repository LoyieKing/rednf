// ---- dispatch_sig ----
// ORIG 0x81616ce。第二参 data 实际承载 Msg*（ORIG 汇编 0x10(%ebp)=data 判空；
// Ghidra C 的 param_3 标注为反编译器参数索引偏差，以汇编为准）。
// 流程：
//   data==0 → 日志 "[Taiwan, pcroom] interMsg is null."，返回 0x19b；
//   user->m_field8ec32 = msg->m_remainCount（movzbl 0x4 → mov %dl,0x8ec32）；
//   remain = movsbl msg->m_remainCount；idStr = NumberToString(msg->m_id, 0)；
//   cMyTrace(trace, 0x19f, 0) 日志 "[Taiwan, pcroom] m_id:%s, remain_reward_count:%d"；
//   SendPacket(user)，返回 0。
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
}