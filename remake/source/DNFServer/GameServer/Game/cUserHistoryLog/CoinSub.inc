void cUserHistoryLog::CoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("Coin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
}