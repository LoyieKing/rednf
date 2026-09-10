unsigned int Dispatcher_BossDieCheck::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    out.m_8 = 0;
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x258b, 0);
        trace("Party == NULL");
        return 8;
    }
    return 0;
