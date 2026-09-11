// ============================================================================
// DB_UpdateClientCrashDownLog::dispatch —— ORIG 0x08427bce（报告 dispatch.md
// 全文）
// ============================================================================
// *stream >> reason(int) → GetInstanceClientCrashDownLogStatistic()->
// RegisterClientCrashDown(reason)；恒返 1。
bool DB_UpdateClientCrashDownLog::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    int reason;
    *stream >> reason;
    GetInstanceClientCrashDownLogStatistic()->RegisterClientCrashDown(reason);
    return 1;
}