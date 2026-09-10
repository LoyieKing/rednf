void BingoEvent::StartEvent()
{
    SetEventFlag(true);
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2a, 0);
        trace("BingoEvent Day(%d)", day);
    }
