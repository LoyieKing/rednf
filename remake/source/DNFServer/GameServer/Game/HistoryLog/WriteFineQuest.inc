void HistoryLog::WriteFineQuest(FILE* file, int questIdx)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_FINE_QUEST, time, questIdx);
