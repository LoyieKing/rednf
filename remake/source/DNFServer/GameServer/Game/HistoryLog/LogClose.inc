void HistoryLog::LogClose(FILE* file)
{
    if (file == 0)
        return;
    fclose(file);
    logfiles_.Free(file);
    count_ -= 1;
}