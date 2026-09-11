void HistoryLog::WriteComposition(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\325\274\272\040\267\271\274\255\307\307\072\040\045\144"
    "\015\012", time, itemIdx);
}