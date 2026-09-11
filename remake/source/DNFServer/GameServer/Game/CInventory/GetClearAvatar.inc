int CInventory::GetClearAvatar(int idx) const
{
    if (idx >= 0 && idx <= 9)
        return m_clearAvatar[idx];
    return 0;
}