int WarAreaCounter::GetWarRoomCountAtPeekTime(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountAtPeekTime(int)", 0x317,
            "WarAreaCounter GetWarRoomCountAtPeekTime error idx(%d)", idx);
        return 0;
    }
    // ORIG: 直读 m_curHourIdx（mov 0xe8(%eax),%edx），无意引局部槽（P0）。
    return m_warRoomCount[m_curHourIdx][idx];
