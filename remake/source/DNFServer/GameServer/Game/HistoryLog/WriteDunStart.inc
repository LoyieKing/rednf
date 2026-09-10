void HistoryLog::WriteDunStart(FILE* file, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DUN_START, time, job, level, growType);
