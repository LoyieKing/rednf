void HistoryLog::WriteBuyItem(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_BUY_ITEM, time, itemIdx, count);
