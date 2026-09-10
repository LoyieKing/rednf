bool HistoryLog::Init()
{
    struct stat st;
    if (stat("./history/", &st) < 0)
    {
        if (mkdir("./history/", 0x1fd) < 0)
        {
            printf("Fail To Make Directory, PATH[%s]\n", "./history/");
            return false;
        }
    }
    return true;
