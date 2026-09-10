void cUserHistoryLog::ExpandInventory(int cap, int newCap, int a)
{
    m_user->LogHistory("ExpandInventory+,%d,%d,%d", cap, newCap, a);
