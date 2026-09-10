void HistoryLog::WriteDie(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DIE, time);
