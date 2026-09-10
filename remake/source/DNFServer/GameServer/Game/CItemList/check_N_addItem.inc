bool CCompound_ExtreamDun_Item::check_N_addItem(CEquipItem& item)   // ORIG 0x832db00
{
    if (_isPossibleResultItem(item) != true)
    {
        return false;
    }
    if (_AddItem(item) != true)
    {
        return false;
    }
    return true;
