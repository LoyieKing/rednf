void HistoryLog::WriteComposition(FILE* file, int itemIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_COMPOSITION, time, itemIdx);
