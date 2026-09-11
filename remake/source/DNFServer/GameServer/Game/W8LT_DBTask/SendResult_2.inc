// ORIG 0x8402aec：结果回包 —— Acquire("DBThread.cpp",0x1271) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0xc)
// → put(DISPATCH_Q,guard) 返 1。
bool DB_DeleteChannel::SendResult(int userIdx, int slot, SIG_DELETE_CHANNEL* sig)
