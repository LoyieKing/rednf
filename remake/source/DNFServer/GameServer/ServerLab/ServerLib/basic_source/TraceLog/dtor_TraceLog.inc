TraceLog::~TraceLog()
{
    close(logfd);
    close(errfd);
    close(statfd);
