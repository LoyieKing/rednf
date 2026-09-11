void HistoryLog::WriteGiveUp(FILE* file, const char* reason)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\306\367\261\342\072\040\045\163\015\012", time, reason);
}