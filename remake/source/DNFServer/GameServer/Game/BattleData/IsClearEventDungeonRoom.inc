char BattleData::IsClearEventDungeonRoom(int param_1, unsigned short roomIdx) const
{
    // b28d-40: N4/P30 封顶——ORIG 0x085be87e 索引链用 %ebx 缓存 roomIdx（push %ebx、
    // 槽 -0x8）且单累加器 add；4.4 用 %eax + `lea (%eax,%edx,1),%edx`（下标序交换
    // 实测无效）。add-vs-lea + callee-saved 选择语义全等。ORIG 0x085be862。
    if (roomIdx >= 3)
    {
        return 0;
    }
    return m_eventClear[param_1 * 3 + roomIdx].m_clearFlag;
}
