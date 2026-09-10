void CInventory::GetClearAvatar(unsigned long* out) const
{
    for (int i = 0; i < 10; i++)
    {
        out[i] = m_clearAvatar[i];
    }
