void cUserHistoryLog::Enchant3rdChronicleItem(bool isEnchant, int itemIdx, int materialIdx,
                                              int equipType, int count)
{
    if (isEnchant)
        m_user->LogHistory("3rdEnchant+,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
    else
        m_user->LogHistory("3rdEnchant-,%d,%d,%d,%d", itemIdx, materialIdx, equipType, count);
