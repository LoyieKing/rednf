void Dispatcher_GrowthCreatureChangeInfinityCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e8768：p4==0 → Ok(0x200)+事件(0x67) IsEventing(+0x34)→SendGrowthCreatureEventInfo；否则 Error
    stGrowthCreature& p = static_cast<stGrowthCreature&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x200);
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
        if (ev != 0 && ev->IsEventing((CUser*)0))
        {
            CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(0x67);
            if (ev2 != 0)
            {
                static_cast<GiveGrowCreatureEvent*>(ev2)->SendGrowthCreatureEventInfo(*user);
            }
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x200,
                                 (unsigned char)(p.m_4 & 0xff));
    }
