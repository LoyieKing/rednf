void CMonster::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                      std::vector<Inven_Item>& out) const
{
    // ORIG 无 call：this+0x60+0xc98=+0xcf8 直算（成员直访折叠，无 script 槽）。
    ::generateDropAvatarItem(m_script.m_dropAvatarInfo, rarity, grade, out);
