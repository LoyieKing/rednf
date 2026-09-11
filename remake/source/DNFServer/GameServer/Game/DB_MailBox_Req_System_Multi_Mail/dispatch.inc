// ============================================================================
// dispatch —— ORIG 0x0844346a（虚覆盖）：GetOutBuffer<SIG_MAILBOX_SEND_SYSTEM_
// MULTI_MAIL> → GetDBHandle(DB_HANDLE_3, +0x395) → getCurSec → localtime_r
// (+1h/min/sec=0) → mktime → 按 +0x390 时段折算 → 写 +0x3a1 → _InsertLetter →
// _InsertPostal（失败返 0）→ sendResult(+0x11e = m_recvCharacNo, +0x395) → 1。
// ============================================================================
bool DB_MailBox_Req_System_Multi_Mail::dispatch(int userIdx, int slot, Stream* stream)
