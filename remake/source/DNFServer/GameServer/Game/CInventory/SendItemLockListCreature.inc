void CInventory::SendItemLockListCreature() const
{
    SendItemLockList(m_pCreatureSlot, 0xf2, (ENUM_ITEMSPACE)7);
}