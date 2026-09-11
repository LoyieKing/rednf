void HistoryLog::WriteSelectCh(FILE* file, const char* name, int job, int level,
                               int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\274\261\305\303\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\045\144\015\012", time, name, job, level, growType);
}