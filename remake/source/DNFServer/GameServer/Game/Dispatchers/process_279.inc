int Dispatcher_Direct_Entrance_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                                    ParamBase& param)
{
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    pref.m_4 = mgr->direct_entrance_quick_party(user);
    return 0;
