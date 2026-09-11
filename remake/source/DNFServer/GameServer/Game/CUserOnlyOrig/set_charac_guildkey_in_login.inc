void CUser::set_charac_guildkey_in_login(unsigned int guildKey, unsigned int characNo)
{
    // ORIG 0x0866c04e（报告 set_charac_guildkey_in_login.md + objdump 核对）。
    // state==2 门控 → 遍历角色列表按 characNo 命中后写 m_guildId(+0xdbe)。
    if (get_state() == 2)
    {
        int i = 0;
        while ((int)m_characList.size() > i)
        {
            if (m_characList[i].m_characNo == (int)characNo)
            {
                m_characList[i].m_guildId = (int)guildKey;
                break;
            }
            ++i;
        }
    }
}