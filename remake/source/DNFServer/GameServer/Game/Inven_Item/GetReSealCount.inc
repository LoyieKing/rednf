unsigned char Inven_Item::GetReSealCount() const
{
    // ORIG 0x822b456：movzbl 0x6 → shr $0x5,%al（位域 hi:3 读）
    return m_attrBits.m_attrHi;
