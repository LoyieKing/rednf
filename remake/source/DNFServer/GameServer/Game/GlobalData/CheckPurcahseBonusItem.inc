// ============================================================================
// CheckPurcahseBonusItem —— ORIG 0x08179ed8 (T, sz 0x30)
// _ZN18ItemVendingMachine22CheckPurcahseBonusItemEm
// 报告 ItemVendingMachine/CheckPurcahseBonusItem.md：仅取 itemIdx 低 32 位作
// key 查 m_bonus（this+0x08，ARAD::CeraShopPurcahseCountBonus）.CheckBonusItem，
// 命中→true。Report C 体即 `CheckBonusItem((int)itemIdx)`。
// ============================================================================
bool ItemVendingMachine::CheckPurcahseBonusItem(unsigned long itemIdx)
{
    // ORIG 0x08179ef3：call 后 xor $1+test+je→mov $1 块 / fall-through mov $0
    // ——bool 物化尾（同 OpenMaster 技巧 1/17 形）。
    if (!m_bonus.CheckBonusItem((int)itemIdx))
        return false;
    return true;
}