void CInventory::SetClearAvatar(unsigned long* arr)
{
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = arr[i];
    }
}