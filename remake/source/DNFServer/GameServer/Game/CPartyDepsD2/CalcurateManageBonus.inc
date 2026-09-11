// ---------------------------------------------------------------------------
// CalcurateManageBonus —— ORIG T 0x085ada30
// 报告：docs/class_func_reports/CParty/CalcurateManageBonus.md
// 返回 st0 浮点；user 为空/角色为空/等级>=最大等级 → 0。
// type=0 → CharacManageScript::GetExpBonus(manageLevel)；
// type=1 → GetGoldBonus(manageLevel) * (double)(uint)param（fildll 无符号扩展）。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] AE（67=67）：P0/P200 去 characLevel 具名 short（内联
// movzwl+movswl %ax,%ebx）、P24/P52 去 manageLevel/dataMgr 缓存、P243 乘序改
// (float)param * GetGoldBonus（fildll+fstps 先落槽）⇒ 帧 0x24 逐条全等。
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
}