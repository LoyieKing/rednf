bool DB_UpdateClientCrashDownLog::dispatch(int /*userIdx*/, int /*slot*/,
                                           Stream* stream)
{
    int reason;
    *stream >> reason;
    GetInstanceClientCrashDownLogStatistic()->RegisterClientCrashDown(reason);
    return 1;
