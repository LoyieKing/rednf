void CPowerWarVictoriousEvent::StartEvent(Word_Param param)
{
    // [N7 封顶] ORIG 虚调用 funcptr→%edx、this→%eax；4.4 恒 funcptr→%eax
    // （探针 /tmp/nl24p p1/p2 三源形全败），寄存器名差封顶。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x34, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
