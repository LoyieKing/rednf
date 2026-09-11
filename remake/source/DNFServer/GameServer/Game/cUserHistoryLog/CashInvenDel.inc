// ORIG _ZN15cUserHistoryLog12CashInvenDelE19eCashInvenDelReasonjjmj @0x08686874
void cUserHistoryLog::CashInvenDel(eCashInvenDelReason reason, unsigned int a,
                                   unsigned int b, unsigned long c, unsigned int d)
{
    m_user->LogHistory("CashInven-,%u,%u,%u,%u,%u",
                       (unsigned)reason, a, b, (unsigned)c, d);
}