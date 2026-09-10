CEventCreateDnf::CEventCreateDnf()
    : m_state(State::kWaiting), m_startHour(0), m_endHour(0),
      m_reloadInterval(0), m_reserved(0)
{
    // ORIG C1 0x08163aa4：只清 +0xc/+0x10/+0x11/+0x12/+0x14；+0x16 m_rate 不初始化。
