bool CInventory::CheckValidSlot(int invenType, int slot) const
{
    if (slot < 0)
    {
        return false;
    }
    switch (invenType)
    {
    case 0:
        if (slot > 0x19)
        {
            return false;
        }
        break;
    case 1:
        if (slot > 0x137)
        {
            return false;
        }
        break;
    case 2:
        if (slot > 0x68)
        {
            return false;
        }
        break;
    case 3:
        if (slot > 0xf1)
        {
            return false;
        }
        break;
    case 5:
        if (slot < 10 || slot > 0x16)
        {
            return false;
        }
        break;
    }
    return true;
}