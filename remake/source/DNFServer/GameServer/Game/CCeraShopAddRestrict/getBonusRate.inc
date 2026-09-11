// getBonusRate（ORIG W 0x0817a2e2）：直返 +0x0c bonusRate（mov 0xc(%eax),%eax;
// ret；P9 直返形态，StartEvent(unsigned short) 以 4 字节写入同一字段）。
int CCeraShopBonusItemEvent::getBonusRate() const
{
    return (int)m_rate;
}