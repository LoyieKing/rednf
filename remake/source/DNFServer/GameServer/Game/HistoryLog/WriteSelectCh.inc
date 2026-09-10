void HistoryLog::WriteSelectCh(FILE* file, const char* name, int job, int level,
                               int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_SELECT_CH, time, name, job, level, growType);
