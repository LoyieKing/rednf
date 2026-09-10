int CUserCharacInfo::calc_disp_help_abuse_ratio(int param)
{
    int levelUpExp = get_level_up_exp(get_charac_level());
    float helpRate =
        (float)((double)getCurCharacDungeonHelpAbuseExp() / (double)levelUpExp);
    int ratio = (int)((float)getCurCharacDungeonHelpAbuseRatio() + 1000000.0f * helpRate);
    int result = 0;
    if (get_charac_level() + param > 0)
        result = ratio / (get_charac_level() + param);
    return std::min(1000000, result);
