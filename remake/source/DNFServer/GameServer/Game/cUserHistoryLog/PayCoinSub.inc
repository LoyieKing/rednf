void cUserHistoryLog::PayCoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("PCoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
