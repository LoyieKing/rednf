void cUserHistoryLog::seal_Item(int itemIdx, short count, eSealItemReason reason)
{
    if (m_user != 0)
        m_user->LogHistory("SealItem ,%d,%d,%d", itemIdx, (int)count, (int)reason);
    return;
