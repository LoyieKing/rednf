// ORIG _ZN15cUserHistoryLog15MercenaryReturnEjiiiii @0x08686b0a（注意 "MecenaryReturn" 拼写照抄 ORIG）
void cUserHistoryLog::MercenaryReturn(unsigned int mercNo, int a, int b, int c, int d, int e)
{
    m_user->LogHistory("MecenaryReturn,%u,%d,%d,%d,%d,%d", mercNo, a, b, c, d, e);
}