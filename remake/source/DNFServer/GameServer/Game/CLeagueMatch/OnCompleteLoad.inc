// ORIG T 0x0855e22c：向全体成员广播加载完成通知（0x81 包，携带 unique id）。
// [b28d-solo-CLeagueMatch] DIFF 97/95·ndiff 22·N-STRIDE（S217 新鲜 .o 复验）：
// 残差 = i*0x1c 的 shl-vs-lea + 0x70 族乘法展开序差（ORIG 先 `shl $4`(t*16)
// 再 `lea(,%x,8);sub`，4.4 先算 i*7 再 shl），逐条语义等价；p167 9/9 CLEAR。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局 · gap=+2 缺口定位] 本件 ORIG 唯一
// ORIG-only 段 = 0x0855e2b7/0x0855e2b9 `mov %ecx,%eax; mov %eax,%ecx`（t 的纯
// 冗余寄存器往返，删去后逐条等价）。4.4.7 探针穷举（/tmp/probe_ocl{1,2,3}：
// 70 源形——数组/指针 deref/`*(TI*)((char*)p+off)`/sizeof/char* 偏移链/两层引用/
// 具名中间量/register/unsigned·cast 索引/上界写法/加法序）恒产
// `mov %edx,%eax; shl $0x4,%eax`（无往返）；追加 `#pragma GCC target`
// (tune=i386/i486/pentium/pentiumpro · arch=i386/i486/pentiumpro · tune=generic)
// 8 形 + `#pragma GCC optimize("strength-reduce"/"O1"/"web")` 3 形 + cc1plus
// **4.4.6 后端** A/B（头集 4.4.6 + i486，独立 .o，逐件读数与本 TU 全同）亦全败
// ⇒ 该 2 条为 ORIG 编译器（4.1 系）synth_mult 的 copy 链产物，4.4 不可翻【已试尽】。
// p167 9/9 CLEAR：MISSING `PacketGuardC2Ev`×1/`D2Ev`×2 ≡ EXTRA `C1Ev/D1Ev`
// （nm ORIG 实证同址别名 0x0858dd4c / 0x0858de80）⇒ 非语义缺口。
void CLeagueMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x81);
    pkt.put_short(user->get_unique_id() & 0xffff);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo&/PlayerInfo& 引用缓存槽
    // （Ghidra 仅 t/i/user 三局部）；以 (&m_teamA)[t] 数组化寻址 + user 具名局部
    // （ORIG -0x10）复现，并消 4.4 的引用槽。
    for (int t = 0; t <= 1; ++t) {
        for (int i = 0; i < 4; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
}