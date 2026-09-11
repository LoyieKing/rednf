// ============================================================================
// 4. DB_Multi_MailBox_Req_Send::dispatch —— ORIG 0x0843bada（dispatch.md 全文）：
//    GetOutBuffer<SIG_MAILBOX_CHECK_RECEIVER> → 到期时间 = 下一整点
//    （getCurSec+1h 清零分秒 → mktime）→ GetDBHandle(DB_HANDLE_3,0) →
//    InsertLetter(db, +0x1d 收件人, +0x2d 发件人, +0x3fc1 name, +0xdb title,
//    +0x3fbd letterId, t, 0)，失败 logFormat(0x8e53) → _sendResult(slot, sig)
//    → 返 1。
// b28c-L20: N1 — 唯一真残差 `sig->m_payload + 0xaa`（m_payload char[0x3f88] @+0x31 ⇒
//   +0xdb）：ORIG 0x0843bb71 `add $0xdb,%eax; mov %eax,%ecx` 两步，4.4 恒折单条
//   `lea 0xdb(%eax),%esi`（数组退化+常量折叠），其余为寄存器名对调（N82附）。ndiff 13。
// ============================================================================
bool DB_Multi_MailBox_Req_Send::dispatch(int userIdx, int slot, Stream* stream)
