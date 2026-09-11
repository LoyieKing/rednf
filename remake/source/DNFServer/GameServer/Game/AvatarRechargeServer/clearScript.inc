// ORIG 0x088b12d8：上限清零 + 三 map clear。
void ARAD::SCRIPT::AvatarRecharge::clearScript()
{
    m_rechargeLimit = 0;
    m_emblemRechargeStoneMap.clear();
    m_emblemRechargeMap.clear();
    m_useEmblemGradeRateMap.clear();
}