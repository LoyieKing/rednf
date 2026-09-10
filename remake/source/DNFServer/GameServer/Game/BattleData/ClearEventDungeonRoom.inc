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
