// ORIG T 0x08683cee（ItemAddCargo.md）："Inven+,%d,%d,%d"（itemIdx, count, slotType）。
void cUserHistoryLog::ItemAddCargo(int itemIdx, int count, int slotType)
{
    m_user->LogHistory("Inven+,%d,%d,%d", itemIdx, count, slotType);
}