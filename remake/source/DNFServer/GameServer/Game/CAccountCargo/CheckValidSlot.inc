bool CAccountCargo::CheckValidSlot(int slot) const
{
    if (slot < 0 || (int)m_capacity <= slot || 0x37 < slot)
        return false;
    return true;
