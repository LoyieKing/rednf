void HistoryLog::WriteDeleteCh(FILE* file, char* name, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DELETE_CH, time, name, job, level, growType);
