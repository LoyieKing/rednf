void cUserHistoryLog::ItemAddCargo(int itemIdx, int count, int slotType)
{
    m_user->LogHistory("Inven+,%d,%d,%d", itemIdx, count, slotType);
