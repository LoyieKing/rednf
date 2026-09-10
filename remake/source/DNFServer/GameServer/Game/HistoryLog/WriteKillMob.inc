void HistoryLog::WriteKillMob(FILE* file, int mobType)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_KILL_MOB, time, mobType);
