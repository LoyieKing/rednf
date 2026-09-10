int Dispatcher_PartyTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stPartyTeleport(Param) pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    unsigned char& echoState = *reinterpret_cast<unsigned char*>(pbase + 0x8); // +0x08 回显
    int& auraRemain = *reinterpret_cast<int*>(pbase + 0xc);       // +0x0c 光环剩余秒

    MSG_PARTY_TELEPORT& req = reinterpret_cast<MSG_PARTY_TELEPORT&>(msg);

    err = check_error(user, msg);
    echoState = req.m_field0d;
    auraRemain = user->get_aura_avatar_option_value(0);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec < auraRemain)
    {
        auraRemain = auraRemain - curSec;
    }
    else
    {
        auraRemain = 0;
    }

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(19260,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        err = 0x13;
        return 0;
    }
    // ORIG：每次操作重取 GetPartyTelePort（P48 无 CSE）。
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->set_teleport_data(req.m_field0d, req.m_field0e,
                            static_cast<short>(req.m_field0f),
                            static_cast<short>(req.m_field11),
                            req.m_field13, '\0');
    int slot = static_cast<int>(party->GetMemberSlotNo(user));
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->set_teleport_member_state(slot, 1);
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->check_cur_teleport_state();
    return 0;
