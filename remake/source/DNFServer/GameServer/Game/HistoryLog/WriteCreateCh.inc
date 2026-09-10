void HistoryLog::WriteCreateCh(FILE* file, char* name, int job)   // ORIG T 0x084b9d70
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_CREATE_CH, time, name, job);
