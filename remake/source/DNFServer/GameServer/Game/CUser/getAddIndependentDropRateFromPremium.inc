// ORIG W 0x0830edaa _ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj
// 转发 m_premium（+0x7121c）独立掉落率查询（参数直传，ORIG 无交换）。
unsigned int CUser::getAddIndependentDropRateFromPremium(ENUM_PREMIUM_TYPE type, unsigned int level)
{
    return m_premium.GetAddIndependentDropRate(type, level);
}