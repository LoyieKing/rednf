void HistoryLog::WriteTradeDown(FILE* file, int itemIdx, int count)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_TRADE_DOWN, time, itemIdx, count);
