char BattleData::IsClearEventDungeonRoom(int param_1, unsigned short roomIdx) const
{
    // ORIG 0x085be862（23 条）：push %ebx / 槽 -0x8 截断 / `movzwl -0x8(%ebp),%ebx`
    // 把 roomIdx 落 callee-saved，索引链 `add %eax,%eax; add %edx,%eax; add %ebx,%eax;
    // add $0xc4,%eax` 单累加器。
    // [b28d-L5a 2026-09-11] 纯 `register` 存储类提示（P231，非 asm 绑定、P0 合规）
    // `register int roomIdxIdx = roomIdx;` 复现 ORIG 的 %ebx 落点 + push/pop %ebx +
    // 槽 -0x8 + `movzwl -0x8(%ebp),%ebx`；指令数 20 → 23 与 ORIG **全等**。
    // 残余（本形不可翻，后端族）：①末步 4.4 折 `lea (%eax,%ebx),%edx` vs ORIG
    // `add %ebx,%eax`（探针 /tmp/l5/p1..p4 共 14 形：拆乘法/交换加法序/具名索引/
    // 数组引用/`t+=r` 原地累加/多 register 变量全试，出 add 的形必多 push %esi +
    // `mov %ebx,%edx` ⇒ 26 条反超）②this 装载 4.4 恒在索引后（%eax）vs ORIG 在
    // 索引前（%ecx）；N18a/N7 族，语义全等。
    if (roomIdx >= 3)
    {
        return 0;
    }
    register int roomIdxIdx = roomIdx;
    return m_eventClear[param_1 * 3 + roomIdxIdx].m_clearFlag;
}
