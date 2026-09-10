bool Inter_PcroomResponse::IsShutdonTimeOverLogin()
{
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTm;
    struct tm* pTm = localtime_r(&curSec, &localTm);
    if (pTm->tm_hour < 6)
        return true;
    return false;
