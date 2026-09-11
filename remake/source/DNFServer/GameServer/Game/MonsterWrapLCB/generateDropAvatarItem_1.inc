// ----------------------------------------------------------------------------
// ORIG T 0x0834a15e _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   lea 0x30(this) 取 m_dropAvatarInfo 后转发自由函数。
void CAICharacter::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                          std::vector<Inven_Item>& out) const
{
    ::generateDropAvatarItem(m_dropAvatarInfo, rarity, grade, out);
}