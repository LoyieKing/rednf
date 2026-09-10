int Dispatcher_CompleteLoadAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // ORIG 0x08206b5c：完整加载后登记 super power 状态定时器并向全队广播确认。
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa151, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->GetCurCharacSuperState() != 0)
    {
        CDataManager* dm = G_CDataManager();
        int sec = dm->m_powerParameter.m_timeAssaultSuperPowerStateAfter;  // CDataManager +0xa688
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x7f, sec,
                                    (int)user->GetLoginTick(),
                                    (unsigned int)user->getCurCharacNo());
    }
    CParty* party = (CParty*)user->GetParty();
    if (party != 0 && party->CheckRecvFromAllMember(user))
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0xf8);
        guard.put_byte(1);
        guard.finalize(true);
        party->send_to_party(guard);
        party->SetRecvResultFlag(true);
    }
    return 0;
