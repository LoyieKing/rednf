void CMissionList_Charac::Remove_Mission(int kind)
{
    // b28c-L5：ORIG 0x085e4bec 两条范围检查各落**独立** nop 中继岛
    // （+0x79 `nop; jmp +0x7d` / +0x7c `nop;` fall 到 +0x7d leave），body 尾另有
    // `jmp +0x7d`。4.4 对 `if ((kind>0)&&(kind<0x24)) {body}` 恒 jump-thread 合并两
    // 失败出口（少 4 条）。探针 /tmp/l5_probe_a.cpp fD 实证：`goto` 双守卫 + 尾标签串
    // 精确产 ORIG 三岛形（P223 多岛扩展 BM8 同法）。语义与原守卫完全一致。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    if (m_user)
    {
        m_user->m_historyLog.pvpMissionDel(
            (int)m_missionList[kind].m_kind,
            (int)m_missionList[kind].m_index);
    }
    m_missionList[kind].clear();
    alter();
L_overrange:;
L_nonrange:;
