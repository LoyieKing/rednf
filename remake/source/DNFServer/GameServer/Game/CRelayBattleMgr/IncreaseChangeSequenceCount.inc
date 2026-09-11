// ============================================================================
// 顺序编排
// ============================================================================
void CRelayBattleMgr::IncreaseChangeSequenceCount(int team)
{
    if (team <= 0)
        return;
    if (team > 2)
        return;
    ++m_changeSeqCount[team];
    if (m_changeSeqCount[team] > 3)
    {
        // [b28c-L18] P0 清绑 + P227 全表达式临时链：ORIG 0x085de4dc 先以 4 条
        // 地址链把 m_changeSeqCount[team] 单段直落 %ebx（`mov (%eax,%edx,4),%ebx`）
        // 跨 cMyTrace ctor 保活，op() 处 `mov %ebx,0x8(%esp)`——具名 trace 对象+
        // 绑定形违规；P227 实证该形 4.4 可在 ctor 前右起求 varargs 并以 %ebx 跨
        // ctor 保活（0x0840215e 先例）。
        cMyTrace(__PRETTY_FUNCTION__, 0x13de, 0)(
            "ERROR ChangeSequenceCount overflow (%d)", m_changeSeqCount[team]);
        m_changeSeqCount[team] = 4;
    }
}