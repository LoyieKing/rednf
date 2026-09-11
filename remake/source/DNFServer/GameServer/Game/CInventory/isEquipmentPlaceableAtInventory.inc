bool CInventory::isEquipmentPlaceableAtInventory(int slot)
{
    if (slot > 2 && slot <= 8)
        return true;
    if (slot > 8 && slot <= 0x38)
        return true;
    return false;
}