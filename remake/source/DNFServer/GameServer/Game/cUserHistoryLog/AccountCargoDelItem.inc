void cUserHistoryLog::AccountCargoDelItem(int itemIdx, int count, int slotType,
                                          eItemDelReason reason)
{
    m_user->LogHistory("AccountCargo-,%d,%d,%d,%d",
                       static_cast<int>(reason), itemIdx, count, slotType);
