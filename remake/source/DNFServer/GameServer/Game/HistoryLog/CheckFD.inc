bool HistoryLog::CheckFD(FILE* file)
{
    if (file == 0)
    {
        return false;
    }
    get_str_datetime(time, 0x10);
    return true;
