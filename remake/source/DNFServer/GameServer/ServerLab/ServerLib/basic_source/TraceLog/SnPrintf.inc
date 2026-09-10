int TraceLog::SnPrintf(char* buf, int nSize, char* des, char* Data, char* Time, char* tmpbuf)
{
    return snprintf(buf, 0x2028, des, cur_date, cur_time, tmpbuf);
