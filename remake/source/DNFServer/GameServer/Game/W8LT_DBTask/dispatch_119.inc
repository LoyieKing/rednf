// ============================================================================
// DB_TerminateServer::dispatch —— ORIG 0x0841a2ee（报告 dispatch.md）。
// push/mov esp/mov $0x1,%eax/pop/ret —— 恒返 1（参数未使用）。
// ============================================================================
bool DB_TerminateServer::dispatch(int userIdx, int slot, Stream* stream)
