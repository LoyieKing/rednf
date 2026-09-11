// [b28d-solo-CLeagueMatch] DIFF 95/93·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（同 OnCompleteLoad），
// 语义等价；p167 8/8 CLEAR。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] gap=+2 缺口 = ORIG 0x0855e2b7 同族
// `mov %ecx,%eax; mov %eax,%ecx` 冗余 copy 链（同一循环体两份各 ×2，共 2 条差），
// 70 形/pragma target 8 形/pragma optimize 3 形/4.4.6 后端 A/B 全败（证据同
// OnCompleteLoad）【不可翻】；p167 8/8 CLEAR（C1/C2·D1/D2 同址别名）。
void CLeagueMatch::_SendMatchStatePacket(int state, int teams)   // ORIG T 0x0855e664
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo& 引用槽（t/i/user 三局部，
    // user 具名缓存 -0xc，寻址 this+i*0x1c+t*0x70+0x24）⇒ 数组化访问复现。
    for (int t = 0; t < teams; ++t) {
        for (int i = 0; i <= 3; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
}