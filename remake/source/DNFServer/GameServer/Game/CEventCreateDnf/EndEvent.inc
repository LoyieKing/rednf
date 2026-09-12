void CEventCreateDnf::EndEvent()
{
    // [R11fixE7 2026-09-12] 手写串 → __PRETTY_FUNCTION__（ORIG prett
    // `virtual void CEventCreateDnf::EndEvent()` raw @0x8b7caa0）：
    // ORIG 用 _ZZN15CEventCreateDnf8EndEventEvE19__PRETTY_FUNCTION__ 静态符号。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x38, 0);
    trace("[Taiwan, CreateDnfEvent] End Event.");
    m_startHour = 0;
    m_endHour = 0;
    m_reloadInterval = 0;
    setState(State::kWaiting);
    SetEventFlag(false);
    // ORIG 0x08163cd0：不清 +0x16 m_rate。
}