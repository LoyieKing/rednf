bool CParty::SendDungeonCharacInfo(CUser* user, bool flag)
{
    // 报告：docs/class_func_reports/CParty/SendDungeonCharacInfo.md（T 0x085b5098）：
    // 游玩时长/等级快照写入 +0x8e034/+0x8e058，收集开关开启时依次发三个统计包。
    user->m_character.m_f00 =
        GlobalData::s_systemTime_.getCurSec() - m_field185c;   // +0x8e034
    user->m_character.m_f24 = user->get_charac_level();        // +0x8e058

    if (user->m_character.m_f20 != 0)                          // +0x8e054 收集开关
        SendDungeonStatisticParty(flag, user);
    if (user->m_character.m_f20 != 0)
        SendDungeonStatisticPartyJob(flag, user);
    if (user->m_character.m_f20 != 0)
        SendDungeonStatisticPartyCharac(flag, user);
    return true;
