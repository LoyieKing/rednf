void HistoryLog::WriteDunStart(FILE* file, int job, int level, int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\040\275\303\300\333\040\112\157\142\072\040"
    "\045\144\054\040\114\145\166\145\154\072\040\045\144\040\274\272\300\345\307"
    "\374\072\040\045\144\015\012", time, job, level, growType);
}