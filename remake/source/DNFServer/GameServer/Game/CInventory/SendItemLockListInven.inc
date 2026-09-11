void CInventory::SendItemLockListInven() const
{
    SendItemLockList(m_pEquipSlot, 0x138, (ENUM_ITEMSPACE)0);
}