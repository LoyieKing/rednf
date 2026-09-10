void HistoryLog::WriteTradeUp(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_TRADE_UP, time, itemIdx, count);
