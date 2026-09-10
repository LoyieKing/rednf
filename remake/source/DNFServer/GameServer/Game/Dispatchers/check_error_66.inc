int Dispatcher_Select_Item_Grwoth_Power::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081db580：state>2 且当前角色非空才放行；随后要求
    // (growth_power_reward & 0xff) < 当前段下界（get_break_away_section_low_level）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    int level = user->get_charac_level();
    int low = G_CDataManager()->m_growthPower.get_break_away_section_low_level(level);
    unsigned char reward = user->get_growth_power_reward();
    if (static_cast<int>(reward) < low)
    {
        return 0;
    }
    return -1;
