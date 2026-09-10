void cUserHistoryLog::AccountCargoAddItem(int itemIdx, int count, int slotType,
                                          eItemAddReason reason)
{
    m_user->LogHistory("AccountCargo+,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
