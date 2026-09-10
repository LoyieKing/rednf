bool CUser::IsEquipAvatar()
{
    if (getCurCharacR() != 0)
        return ((const CInventory*)getCurCharacInvenR())->IsEquipAvatar();
    return false;
