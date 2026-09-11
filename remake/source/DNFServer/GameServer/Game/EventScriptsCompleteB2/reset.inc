// ORIG W 0x08985ae0：字节+8=0 → vec<pair>+0xc clear → vec<int>+0x18 clear →
// mail+0x24 reset。
void ExchangeRandomItemRewardScript::reset()
{
    m_flag08 = 0;
    m_pairs0c.clear();
    m_ints18.clear();
    m_mail.reset();
}