// ---- is_notify —— ORIG 0x0827d29d ----
// premium 项存在且 m_state==2（等待开始）时返回 1。
unsigned char premium_helper_function::is_notify(CUser* user, int type)
{
    // ORIG 0x0827d29d：链式内联（GetPremiumInfo 结果不入槽）；m_type 槽直读
    // 单次，m_state==2 用 == 直比（三目会折 cmp 双物化，异形）。
    WongWork::SUserPremiumInfo* info =
        user->GetPremiumInfo()->GetPremiumInfo(type);
    if (info->m_type == type)
        return info->m_state == 2;
    return 0;
}