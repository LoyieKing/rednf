// ---------------------------------------------------------------------------
// ORIG 0x08268986：IsEventing(0) 为假时置事件标志、挂 0x8b 定时器
// （时长 = m_victoriousRewardTime × 0x3c 分钟）并打日志。
void CPowerWarVictoriousEvent::StartEvent()
{
    // [N7 封顶] IsEventing 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
}
