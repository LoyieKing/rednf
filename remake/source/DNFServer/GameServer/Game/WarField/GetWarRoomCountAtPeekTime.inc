// ORIG 0x82a3d80 W（_ZN14WarAreaCounter25GetWarRoomCountAtPeekTimeEi）：
// 返回当前时段（m_curIdx）第 idx 个 WarRoom 区的目标房间数；idx 越界记日志返 0。
int WarAreaCounter::GetWarRoomCountAtPeekTime(int idx)
{
    if (idx < 0 || idx > 9)
    {
        // [Fn28Gap 2026-09-11] ORIG 0x082a3daa: movl $0x8c19e4f,0x8(%esp) =
        // _ZZN14WarAreaCounter25GetWarRoomCountAtPeekTimeEiE12__FUNCTION__
        // （r,26B="GetWarRoomCountAtPeekTime\0"）⇒ 原码 __FUNCTION__。
        LogManager::logFormat(1, "WarField.cpp",
            __FUNCTION__, 0x317,
            "WarAreaCounter GetWarRoomCountAtPeekTime error idx(%d)", idx);
        return 0;
    }
    // ORIG: 直读 m_curHourIdx（mov 0xe8(%eax),%edx），无意引局部槽（P0）。
    return m_warRoomCount[m_curHourIdx][idx];
}