// ORIG 0x82a3df4 W（_ZN14WarAreaCounter25GetWarRoomCountFirstIndexEi）：
// 当前时段前 idx 个区的计数和（供 WarRoom 扩容定位首索引）。
int WarAreaCounter::GetWarRoomCountFirstIndex(int idx)
{
    if (idx < 0 || idx > 9)
    {
        // [Fn28Gap 2026-09-11] ORIG 0x082a3e1e: movl $0x8c19e82,0x8(%esp) =
        // _ZZN14WarAreaCounter25GetWarRoomCountFirstIndexEiE12__FUNCTION__（26B）。
        // [R11fixE5] ORIG 0x082a3e26 file=0x8c1611c、0x082a3e0e fmt=0x8c16150
        // ——ORIG 此处**照抄了 LastIndex 的文案**（复制粘贴缺陷，raw 逐字节证实），
        // 按 ORIG 原样复刻：'GetWarRoomCountLastIndex Invalid Parameter levelGrid<%d>'。
        LogManager::logFormat(1, "../../DNFShared/GameScript/RDARScriptEtcParameter.h",
            __FUNCTION__, 0x320,
            "GetWarRoomCountLastIndex Invalid Parameter levelGrid<%d>", idx);
        return 0;
    }
    int sum = 0;
    for (int j = 0; j < idx; ++j)
    {
        // ORIG: 循环内直读 m_curHourIdx（不 hoist 到局部槽，P0）。
        sum += m_warRoomCount[m_curHourIdx][j];
    }
    return sum;
}