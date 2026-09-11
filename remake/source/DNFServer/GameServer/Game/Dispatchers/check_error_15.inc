// process 依赖（清单外）：清 param.m_8=0；GetPartyIndex 找不到队伍 → cMyTrace
// "Party == NULL"，返回 8；否则 0。（声明需入 Dispatchers.h，见 NEEDS_DECL）
int Dispatcher_BossDieCheck::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
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
}