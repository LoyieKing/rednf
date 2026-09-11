// QPAdd(int, int, int)：ORIG _ZN15cUserHistoryLog5QPAddEiii @0x0868888c。
// 记录 "QP+,%d,%d,%d"（before, questIdx, add）。
void cUserHistoryLog::QPAdd(int before, int questIdx, int add)
{
    m_user->LogHistory("QP+,%d,%d,%d", before, questIdx, add);
}