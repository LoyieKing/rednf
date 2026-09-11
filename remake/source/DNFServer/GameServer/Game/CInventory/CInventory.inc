// ORIG C1 0x822d52c W：26 个 m_invenItem 逐元素构造（+0x1c 步进 0x3d）→
// m_pEquipSlot(+0x650)/m_pAvatarSlot(+0x654)/m_pCreatureSlot(+0x6e4) 置 0 →
// AvatarCoin C1(+0x658) → CCreatureMgr C1(+0x6e8) → CAvatarItemMgr C1(+0x734)
// → reset()。成员初始化列表按声明序即复现该序列。
CInventory::CInventory()
    : m_invenItem(), m_pEquipSlot(0), m_pAvatarSlot(0), m_avatarCoin(),
      m_pCreatureSlot(0), m_creatureMgr(), m_avatarItemMgr()
{
    reset();
}