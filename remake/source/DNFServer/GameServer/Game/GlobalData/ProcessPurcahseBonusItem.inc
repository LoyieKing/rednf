// ============================================================================
// ProcessPurcahseBonusItem —— ORIG 0x08179f08 (T)
// _ZN18ItemVendingMachine24ProcessPurcahseBonusItemEP5CUsermit
// 纯委托：m_bonus（ARAD::CeraShopPurcahseCountBonus，this+0x08）.
// GiveBonusItem(user, itemIdx, count, threshold)（ORIG 0x081abf44，已定义
// 于 Arad_ItemBonus.cpp）。报告 ProcessPurcahseBonusItem.md 全文转写。
// ============================================================================
void ItemVendingMachine::ProcessPurcahseBonusItem(CUser* user, unsigned long itemIdx,
                                                  int count, unsigned short threshold)
{
    m_bonus.GiveBonusItem(user, itemIdx, count, threshold);
}