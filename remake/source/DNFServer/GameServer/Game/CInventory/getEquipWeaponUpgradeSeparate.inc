unsigned char CInventory::getEquipWeaponUpgradeSeparate() const
{
    // ORIG 以绝对偏移访问（+0x280 标志、+0x2b1 UpgradeSeparateInfo），逐指令还原。
    if (m_invenItem[10].m_addInfo == 0)
    {
        return 0;
    }
    return m_invenItem[10].m_upgradeSep.GetUpgradeSeparate();
