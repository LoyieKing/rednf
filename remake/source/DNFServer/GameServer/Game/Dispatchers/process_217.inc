int Dispatcher_PartyTeleportConfirm::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stPartyTeleportConfirm& p =
        static_cast<stPartyTeleportConfirm&>(param);
    p.m_result = check_error(user, msg);
    if (p.m_result > 0)
    {
        return 0;
    }
    if (p.m_result < 0)
    {
        return LineFunc(
            0x4be5,
            __PRETTY_FUNCTION__,
            p.m_result, 0);
    }
    MSG_PARTY_TELEPORT_CONFIRM& m =
        reinterpret_cast<MSG_PARTY_TELEPORT_CONFIRM&>(msg);
    p.m_bConfirm = m.m_confirmFlag;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        const int slotNo = party->GetMemberSlotNo(user);
        if (m.m_confirmFlag == 1)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_member_state(slotNo, 1);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->check_cur_teleport_state();
        }
        else if (m.m_confirmFlag == 0)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_member_state(slotNo, 0);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_state(2);
        }
        else if (m.m_confirmFlag == 3)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_state(3);
        }
    }
    return 0;
}