// ORIG _ZN15cUserHistoryLog13RedeemItemDelEii @0x086869bc（格式串 ORIG 0x8cf0e9d）
void cUserHistoryLog::RedeemItemDel(int a, int b)
{
    m_user->LogHistory("RedeemItem-,%d,%d", a, b);
}