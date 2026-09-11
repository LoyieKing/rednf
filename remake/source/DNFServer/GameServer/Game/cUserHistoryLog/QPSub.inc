// ORIG _ZN15cUserHistoryLog5QPSubEiii @0x0868885a
void cUserHistoryLog::QPSub(int before, int questIdx, int sub)
{
    m_user->LogHistory("QP-,%d,%d,%d", before, questIdx, sub);
}