int TimerUpdatePvPGrade::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x86373c6：makeRequest() → getCurSec() → registNextTimer(sec+0x15180)。
    DB_UpdatePvPGrade::makeRequest();
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    return 1;
