void UpgradeSeparateInfo::SetUpgradeSeparate(unsigned char value)
{
    // ORIG 0x84e906a：位域 m_lo 装填（value 截断 and + 装填截断 and 双截断形）。
    m_bits.m_lo = value;
}