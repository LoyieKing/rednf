int WarAreaCounter::GetWarRoomCountLastIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        LogManager::logFormat(1, "WarField.cpp",
            "int WarAreaCounter::GetWarRoomCountLastIndex(int)", 0x32f,
            "WarAreaCounter GetWarRoomCountLastIndex error idx(%d)", idx);
        return 0;
    }
    int sum = 0;
    for (int j = 0; j <= idx; ++j)
    {
        // ORIG: 循环内直读 m_curHourIdx（P0）；尾部 sub $0x1（真语义 bug：返 sum-1）。
        sum += m_warRoomCount[m_curHourIdx][j];
    }
    return sum - 1;
