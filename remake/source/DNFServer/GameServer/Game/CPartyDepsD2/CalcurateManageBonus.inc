float CParty::CalcurateManageBonus(ManageBonusType::T type, CUser* user,
                                   unsigned int param)
{
    if (user == 0)
        return 0.0f;                                    // fldz
    if (user->getCurCharacR() == 0)
        return 0.0f;                                    // getCurCharacR==0

    // [b28d-solo-CPartyDepsD2] P0/P200：ORIG 无 characLevel 槽
    // （movzwl 0x27(%eax),%eax; movswl %ax,%ebx）⇒ 内联表达式直用。
    // movzwl 0x27(%eax)：_Charac_info::m_level(+0x27, CUser.h)
    if (!(user->getCurCharacR()->m_level < user->GetUserMaxLevel()))
        return 0.0f;

    if (type == ManageBonusType::T_0)
    {
        // CDataManager+0x6394 = advancealtar::ConfigParameter+0x30
        //                     = m_altarConfig.m_characManageScript
        return G_CDataManager()->m_altarConfig.m_characManageScript
            ->GetExpBonus(user->GetManageLevel());
    }
    if (type == (ManageBonusType::T)1)
    {
        // ORIG 先算左操作数 (float)param（fildll+fstps 落槽）再算右调用链，
        // 末条 fmuls 内存操作数 ⇒ 左为 param 转换（P243 书写序判据）。
        return static_cast<float>(param) *
               G_CDataManager()->m_altarConfig.m_characManageScript
                   ->GetGoldBonus(user->GetManageLevel());
    }
    return 0.0f;
