void CEventCreateDnf::StartEvent(Word_Param param)
{
    // b28d-D42 + round1 R19：ORIG 0x08163be0 逐字节读参（`lea 0xc(%ebp),%eax;
    // mov %eax,-0xc(%ebp); movzbl (%eax),%edx; movzbl 0x1(%eax),%edx;
    // movzbl 0x2(%eax),%eax`）⇒ P244 具名指针槽 + P311 成员视图（`p[i]` 下标形
    // 被 4.4 折成 `add $0xN,%eax` 两条，成员视图才出 disp 单条）。
    struct WordParamBytes
    {
        unsigned char m_b0;  // +0x00 → m_startHour
        unsigned char m_b1;  // +0x01 → m_endHour
        unsigned char m_b2;  // +0x02 → m_reloadInterval（分钟数）
        unsigned char m_b3;  // +0x03（ORIG 未读）
    };
    const WordParamBytes* p = reinterpret_cast<const WordParamBytes*>(&param);
    m_startHour = p->m_b0;
    m_endHour = p->m_b1;
    m_reloadInterval = static_cast<unsigned short>(p->m_b2 * 60);
    if (m_startHour < 24 && m_endHour < 24 &&
        m_reloadInterval > 59 && m_reloadInterval < 3601)
        StartEvent();
    else
    {
        // ORIG 0x08163cb6：LogManager::logFormat + 虚调 EndEvent()
        LogManager::logFormat(
            1, "localtaiwan/Event/EventCreateDnf.cpp",
            __PRETTY_FUNCTION__, 0x31,
            "[Taiwan, CreateDnfEvent] Error! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
            (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
        EndEvent();
    }
