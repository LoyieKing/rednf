void HistoryLog::WriteCreateCh(FILE* file, char* name, int job)   // ORIG T 0x084b9d70
{
    if (!CheckFD(file)) return;
    fprintf(file, "[%s]\xc4\xb3\xb8\xaf\xbb\xfd\xbc\xba:%s \xc1\xf7\xbe\xf7:%d\r\n",
            time, name, job);
}