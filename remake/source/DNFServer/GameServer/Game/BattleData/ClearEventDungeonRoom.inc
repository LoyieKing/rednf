// b28d-40: 真 bug 已修（st.m_monsterIndex >= param_2，原写 <= 反向；ORIG 0x085be82b
// `cmp 0x10(%ebp),%eax; jl fail`）+ P183 去 idx 缓存 + 共享 fail 出口复现；
// 本文档轮（R25DungeonPvpBattleData）实测：goto 形生 3 条出口中继岛（nop+jmp，我方 64 条）
// ⇒ 改「正极性早返 + 嵌套 if 正向包裹」（P214①/P222）：`jbe 主体; mov $0,%eax; jmp epi`
// 早返岛与三个 `jne fail` 共享 fail 块（mov $0,%eax 落尾部）逐块与 ORIG 同构，60 条 vs 61。
// 残差 = ①ORIG 用 %ebx 缓存 roomIndex（push/pop %ebx 各 1，callee-saved 角色 N7/N125）
//        ②3*p1 强度归约 ORIG `add %eax,%eax; add %edx,%eax` vs 4.4 `add; lea (%eax,%edx,1)`
//        ⇒ N86/N125 族，源形不可择（P321-T5：装载序/lea-vs-add 不得用类型旋钮赌）。
// ORIG 0x085be7aa。
int BattleData::ClearEventDungeonRoom(
    int param_1, int param_2, const CMDPacketStruct::_STReqEventDungeonClearRoom& st)
{
    // b28d-40: 真语义 bug 修正——ORIG 0x085be82b `cmp 0x10(%ebp),%eax; jl fail`
    // ⇒ 成功条件是 st.m_monsterIndex >= param_2（原码写成 <= 反向）。
    // 同时按 P183 去 idx 缓存（ORIG 三次独立全量重算索引）、P133 拆独立守卫。
    // [b28d-L5a 2026-09-11] P231 纯 `register` 提示（非 asm 绑定、P0 合规）复现 ORIG
    // 0x085be7cf/0x085be7fd/0x085be83a 三处 `movzwl %ax,%ebx`（roomIdx 落 callee-saved）
    // 与 `push %ebx` 帧。残余 lea-vs-add/寄存器角色属 N18a/N7 后端族（探针 /tmp/l5/p1..p4）。
    if (st.m_roomIndex > 2)
        return 0;
    {
        register int roomIdx1 = st.m_roomIndex;
        if (m_eventClear[param_1 * 3 + roomIdx1].m_monsterIdx == st.m_monsterIndex)
        {
            register int roomIdx2 = st.m_roomIndex;
            if (m_eventClear[param_1 * 3 + roomIdx2].m_roomIdx == st.m_monsterCount)
            {
                if (st.m_monsterIndex >= param_2)
                {
                    register int roomIdx3 = st.m_roomIndex;
                    m_eventClear[param_1 * 3 + roomIdx3].m_clearFlag = 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}
