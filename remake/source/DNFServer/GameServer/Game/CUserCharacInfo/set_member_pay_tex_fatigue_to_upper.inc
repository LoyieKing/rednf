void CUserCharacInfo::set_member_pay_tex_fatigue_to_upper(int fatigue)
{
    if (m_selected)
    {
    CUR->m_memberPayTexFatigue = fatigue;
    }
}