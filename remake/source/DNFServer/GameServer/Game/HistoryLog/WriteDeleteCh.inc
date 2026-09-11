void HistoryLog::WriteDeleteCh(FILE* file, char* name, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\273\350\301\246\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\040\045\144\015\012", time, name, job, level, growType);
}