void CInventory::SendItemLockListAvatar() const
{
    SendItemLockList(m_pAvatarSlot, 0x69, (ENUM_ITEMSPACE)1);
