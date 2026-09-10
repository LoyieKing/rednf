int Dispatcher_Register_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    MSG_QUICK_PARTY& mref = reinterpret_cast<MSG_QUICK_PARTY&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    QuickParty::STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = mref.m_0e;
    key.m_dungeonDiff = mref.m_10;
    CParty* party = (CParty*)user->GetParty();
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    pref.m_4 = mgr->match_proper_party(key, party, user);
    return 0;
