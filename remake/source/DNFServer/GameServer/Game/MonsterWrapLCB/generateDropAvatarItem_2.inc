// ----------------------------------------------------------------------------
// ORIG T 0x0834f8a6 _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0xcf8 —— 即 m_script(+0x60) 内
//   m_dropAvatarInfo(+0xc98) 子对象，具名访问后转发自由函数。
void CMonster::generateDropAvatarItem(unsigned char rarity, unsigned char grade,
                                      std::vector<Inven_Item>& out) const
{
    // ORIG 无 call：this+0x60+0xc98=+0xcf8 直算（成员直访折叠，无 script 槽）。
    ::generateDropAvatarItem(m_script.m_dropAvatarInfo, rarity, grade, out);
}