void cUserHistoryLog::EventCoinSub(int coin, int sub, eCoinSubReason reason)
{
    m_user->LogHistory("ECoin-,%d,%d,%d", coin, sub, static_cast<int>(reason));
