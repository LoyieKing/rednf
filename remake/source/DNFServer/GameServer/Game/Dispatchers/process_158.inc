int Dispatcher_Cancel_Quick_Party::process(CUser* user, MSG_BASE& msg,
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
    CParty* party = (CParty*)user->GetParty();
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    // X1/X2 配方（OPMC8 提供、probe px2 t8/t11 实证）：4.4 复现 ORIG
    // `xor $0x1,%eax; test %al,%al` 的唯一写法是「绑 %eax 的具名 char 局部 + ^= 1」；
    // 裸 `!call()` 只在 callee 返回 bool 时直落，`!= 1`/`== 1` 恒被折成 cmpb $1。
    unsigned char cancelBad =
        (unsigned char)mgr->cancel_quick_party(party, true, user);
    cancelBad ^= 1;
    if (cancelBad)
    {
        pref.m_4 = 1;
    }
    return 0;
