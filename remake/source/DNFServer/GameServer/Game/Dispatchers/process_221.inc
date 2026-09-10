int Dispatcher_Register_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    // ORIG 槽序：err(-0x14) → msg 引用(-0x10) → param 引用(-0xc) → key 对象(-0x18 起)；
    //   原先把 pref/mref 声明在 err 之前，两个引用各上移一格并把帧撑到 0x38（ORIG 0x24）。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_QUICK_PARTY& mref = reinterpret_cast<MSG_QUICK_PARTY&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    stQuickPartyParam& pref = static_cast<stQuickPartyParam&>(param);  // 裸偏移→具名字段（同偏移同宽度）
    QuickParty::STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = mref.m_0e;
    key.m_dungeonDiff = mref.m_10;
    CParty* party = (CParty*)user->GetParty();
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    pref.m_4 = mgr->match_proper_party(key, party, user);
    return 0;
