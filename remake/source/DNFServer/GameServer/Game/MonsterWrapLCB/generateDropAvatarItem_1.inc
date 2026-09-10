void CAICharacter::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                          std::vector<Inven_Item>& out) const
{
    ::generateDropAvatarItem(m_dropAvatarInfo, rarity, grade, out);
