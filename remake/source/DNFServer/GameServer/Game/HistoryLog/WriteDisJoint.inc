void HistoryLog::WriteDisJoint(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\330\303\274\040\111\104\072\040\045\144\015\012", time, itemIdx);
}