void cUserHistoryLog::PayCoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("PCoin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}