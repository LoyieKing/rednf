void HistoryLog::WriteLevelUp(FILE* file, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_LEVEL_UP, time, level);
