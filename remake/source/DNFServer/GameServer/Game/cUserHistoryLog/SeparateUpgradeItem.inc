void cUserHistoryLog::SeparateUpgradeItem(int itemIdx, int from, int to,
                                          bool isUpgrade)
{
    if (isUpgrade)
        m_user->LogHistory("SeparateUpgrade+,%d,%d,%d", itemIdx, from, to);
    else
        m_user->LogHistory("SeparateUpgrade-,%d,%d,%d", itemIdx, from, to);
