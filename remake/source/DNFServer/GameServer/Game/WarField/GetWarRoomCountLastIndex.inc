// ORIG 0x82a3e90 W（_ZN14WarAreaCounter24GetWarRoomCountLastIndexEi）：
// 当前时段前 idx+1 个区的计数和（供 WarRoom 收缩定位末索引）。
int WarAreaCounter::GetWarRoomCountLastIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        // [Fn28Gap 2026-09-11] ORIG 0x082a3eba: movl $0x8c19e69,0x8(%esp) =
        // _ZZN14WarAreaCounter24GetWarRoomCountLastIndexEiE12__FUNCTION__（25B）。
        LogManager::logFormat(1, "WarField.cpp",
            __FUNCTION__, 0x32f,
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
}