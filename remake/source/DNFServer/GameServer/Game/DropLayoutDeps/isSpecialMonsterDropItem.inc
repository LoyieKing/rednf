bool CItem::isSpecialMonsterDropItem()
{
    return (unsigned char)m_drop.m_enable;  // ORIG 0x8539e00：movzbl 0xb4(%eax) 直返
}