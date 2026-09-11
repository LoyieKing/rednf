void HistoryLog::WriteDunClear(FILE* file)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\040\305\254\270\256\276\356\015\012", time);
}