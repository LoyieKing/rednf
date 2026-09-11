void HistoryLog::WriteCreateCh(FILE* file, char* name, int job)   // ORIG T 0x084b9d70
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\273\275\274\272\072\045\163\040\301\367\276"
    "\367\072\045\144\015\012", time, name, job);
}