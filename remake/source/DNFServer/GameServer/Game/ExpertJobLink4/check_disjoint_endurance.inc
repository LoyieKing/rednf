// check_disjoint_endurance（报告 check_disjoint_endurance.md，ORIG T 0x085d1f24）
bool CDisjointer::check_disjoint_endurance()
{
    if (((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() <= 0)
        return false;
    else
        return true;
}