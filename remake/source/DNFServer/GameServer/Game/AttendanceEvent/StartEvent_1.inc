void AttendanceEvent::StartEvent(Word_Param param)
{
    m_addInfo.m_info0 = param.m_duration;
    m_addInfo.m_info1 = param.m_field2;
    if ((short)m_addInfo.m_info0 < 1)
    {
        LogManager::logFormat(
            1, "AttendanceEvent.cpp",
            __PRETTY_FUNCTION__, 0x19,
            "Attendance(AddInfo:%d,%d) Event Start Error!",
            (int)(short)m_addInfo.m_info0, (int)(short)m_addInfo.m_info1);
    }
    else
    {
        StartEvent();
        // P37 链式临时：实参提前物化 esi/ebx 跨 cMyTrace 构造存活（ORIG 同形）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d, 0)(
            "Attendance(AddInfo:%d,%d) Event Start!",
            (int)(short)m_addInfo.m_info0, (int)(short)m_addInfo.m_info1);
    }
