void cUserHistoryLog::CashInvenAdd(eCashInvenAddReason reason, unsigned int a,
                                   unsigned int b, unsigned long c)
{
    m_user->LogHistory("CashInven+,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c);
