void HistoryLog::WriteChat(FILE* file, int type, char* content, int level)
{
    if (!CheckFD(file)) return;
    fprintf(file, FMT_CHAT, time, type, content);
