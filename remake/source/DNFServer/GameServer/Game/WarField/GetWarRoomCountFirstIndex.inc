int WarAreaCounter::GetWarRoomCountFirstIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountFirstIndex(int)", 0x320,
            "WarAreaCounter GetWarRoomCountFirstIndex error idx(%d)", idx);
        return 0;
    }
    int sum = 0;
    for (int j = 0; j < idx; ++j)
    {
        // ORIG: 循环内直读 m_curHourIdx（不 hoist 到局部槽，P0）。
        sum += m_warRoomCount[m_curHourIdx][j];
    }
    return sum;
