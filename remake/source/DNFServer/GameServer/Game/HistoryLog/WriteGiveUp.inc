void HistoryLog::WriteGiveUp(FILE* file, const char* reason)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_GIVE_UP, time, reason);
