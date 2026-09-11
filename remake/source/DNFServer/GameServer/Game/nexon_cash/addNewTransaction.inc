// ORIG 0x081b15ca：printf(" addNewTx: %p\n", tx)（fmt @0x8bb55c0 原样）+ push_back。
//   [Q8 2026-09-11 §6.5 串修复] ORIG bytes @file-off 11982272 = ` addNewTx: %p\n\0`
//   —— **含前导空格**；我方原写无空格，已补（运行期 debug 日志文本逐字节对齐）。
void UserBalance::addNewTransaction(Transaction* tx)
{
    printf(" addNewTx: %p\n", tx);
    m_transactions.push_back(tx);
}