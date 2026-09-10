void HistoryLog::WriteSellItem(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_SELL_ITEM, time, itemIdx, count);
