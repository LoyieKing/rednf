void cUserHistoryLog::CoinAdd(int coin, int amount, eCoinAddReason reason)
{
    m_user->LogHistory("Coin+,%d,%d,%d", coin, amount, static_cast<int>(reason));
}