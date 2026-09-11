// ORIG 0x8110b0a：8 条形 = 位域 m_flag:1 读取 + unsigned char 返回（probe u1 全等）。
unsigned char UpgradeSeparateInfo::IsTradeRestriction() const
{
    return m_bits.m_flag;
}