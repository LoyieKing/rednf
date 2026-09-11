void HistoryLog::WriteLevelUp(FILE* file, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\267\271\272\247\276\367\072\040\045\144\015\012", time, level);
}