void cUserHistoryLog::EventCoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("ECoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
