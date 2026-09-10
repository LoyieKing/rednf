void CEventCreateDnf::StartEvent()
{
    SetEventFlag(true);
    setState(State::kWaiting);
    RegistStartTimer(true);
    // b28d-D42 verdict=AE(IDENTICAL) ORIG 0x08163b36 orig/ours=46/46。
    // 修法 P227/P87：ORIG 在 cMyTrace ctor 之前右起求 varargs
    // 并保活 callee-saved（edi=reloadInterval、esi=endHour、ebx=startHour），
    // 具名 trace 两语句形使 4.4 退化为栈槽 ⇒ 全表达式临时链复现。
    cMyTrace("CEventCreateDnf::StartEvent()", 0x1d, 0)(
        "[Taiwan, CreateDnfEvent] Start! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
        (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
