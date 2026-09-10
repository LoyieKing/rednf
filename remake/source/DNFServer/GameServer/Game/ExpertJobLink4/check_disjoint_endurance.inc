bool CDisjointer::check_disjoint_endurance()
{
    if (((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() <= 0)
        return false;
    else
        return true;
