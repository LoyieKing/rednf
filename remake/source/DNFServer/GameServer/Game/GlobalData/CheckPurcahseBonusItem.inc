bool ItemVendingMachine::CheckPurcahseBonusItem(unsigned long itemIdx)
{
    // ORIG 0x08179ef3：call 后 xor $1+test+je→mov $1 块 / fall-through mov $0
    // ——bool 物化尾（同 OpenMaster 技巧 1/17 形）。
    if (!m_bonus.CheckBonusItem((int)itemIdx))
        return false;
    return true;
