int TraceLog::MakeDir(char* DirName)
{
    return mkdir(DirName, 0x1c0);
