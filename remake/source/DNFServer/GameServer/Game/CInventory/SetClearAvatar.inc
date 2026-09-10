void CInventory::SetClearAvatar(int idx, int value)
{
    if (idx >= 0 && idx <= 9)
    {
        m_clearAvatar[idx] = value;
    }
