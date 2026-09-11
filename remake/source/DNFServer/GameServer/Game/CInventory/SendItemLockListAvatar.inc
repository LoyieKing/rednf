// ORIG 0x084fafbe / 0x084fafee：分别把 m_pAvatarSlot(0x69 槽)/m_pCreatureSlot(0xf2 槽)
// 的锁清单通过 SendItemLockList 发包（ENUM_ITEMSPACE 1=avatar、7=creature）。
void CInventory::SendItemLockListAvatar() const
{
    SendItemLockList(m_pAvatarSlot, 0x69, (ENUM_ITEMSPACE)1);
}