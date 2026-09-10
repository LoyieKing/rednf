TraceLog::TraceLog()
    : logmask(0)
{
    write_to_logserver = false;
    bNeedToLeaveErrorLogForOverSize = true;
