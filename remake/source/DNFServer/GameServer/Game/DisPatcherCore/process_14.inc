int DisPatcher_WalkoutParty::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return errNo;
    }
    if (errNo < 0)
    {
        return -1;
    }

    // ORIG +0x40：槽产生序为 msg 视图(-0x14) → param 视图(-0x10) → 立即
    // 写 param+4=0；两调用点各自内联重算 (int)(char)msg[0xd]（无缓存槽）。
    const MSG_WALKOUT_PARTY& req = *reinterpret_cast<const MSG_WALKOUT_PARTY*>(&msg);
    stWalkoutPartyParam& out = static_cast<stWalkoutPartyParam&>(param);
    out.m_errNo = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        out.m_errNo = 0x13;
        return 0;
    }
    party->SendWalkout((int)(char)req.m_walkoutSlot, ENUM_WALKOUT_TYPE_0);
    out.m_errNo = party->walkout_member(user, (int)(char)req.m_walkoutSlot);
    return 0;
