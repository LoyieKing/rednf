// ORIG 0x816bda0：SetEventFlag(true)；实参（interval→esi、reward→ebx）在
// cMyTrace 构造前物化（P37 链式临时同形）；__PRETTY_FUNCTION__ 符号压参。
void CEventStayTime::StartEvent()
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 10, 0)(
        "[Taiwan, StayEvent] Start! StayTime:%dMin, CheckTime:%dMin",
        m_rewardStayTime, m_intervalStayTime);
    RegistNextTimer();
}