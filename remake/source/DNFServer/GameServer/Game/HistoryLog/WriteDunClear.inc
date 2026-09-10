void HistoryLog::WriteDunClear(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_DUN_CLEAR, time);
