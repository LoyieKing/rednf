void ItemVendingMachine::ProcessPurcahseBonusItem(CUser* user, unsigned long itemIdx,
                                                  int count, unsigned short threshold)
{
    m_bonus.GiveBonusItem(user, itemIdx, count, threshold);
