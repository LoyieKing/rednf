// ---------------------------------------------------------------------------
// ORIG T 0x82ebb9c (size 0xc7)：_ProcessChaos(int killer, int victim)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF 封 N（P321-T5）：已修 P226 语句表达式
//   sete 守卫 + P248 档② bool 字段（m_chaosFlags）+ `>0` 极性（ORIG 0x082ebd32
//   `setg` 复现）；残差 5 组全为装载序/寄存器角色，T5 禁改类型：
//   ① 0x082ebbd2 `cmp %edx,%eax` GetTeam 角色互换（删 (int) cast 无效；交换
//      操作数引 push %ebx 劣化已回退）——N18b/N7；
//   ② 0x082ebc38 `mov -0xc,%eax` 先于 `movl $0xa,0x4(%esp)`（4.4 常量先）——P75；
//   ③ 0x082ebc63 make_basic_info `lea→eax; movl $0; mov %eax,0x4(%esp); mov this,%eax`
//      求值序（4.4 先载 this）——P75；
//   ④ 0x082ebd51 send_all 的 `mov -0xc,%edx` 在 G_GameWorld call 之后（4.4 提前
//      升格 %ebx 跨 call）——P52/N7；
//   ⑤ 4.4 尾部多 2 条 jmp（if/else+外层块尾，ORIG 直落 epilogue）——N121 族。
//   p167 CLEAR。ORIG 0x082ebb9c
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N7）→ 试=①prett2/修正判据缺口 0、
//   strall 串差 0、p167 CLEAR；②446/446h 后端变体四数逐项相同；③本波复核 125/130
//   gap=-5 的 5 组残差 = 实参装载序（`movl $0xa,0x4(%esp)` 常量先 vs 我方载参先）、
//   make_basic_info 求值序、send_all 的 %ebx 提前升格、尾部 2 条 jmp —— 全属
//   4.1↔4.4 求值序/寄存器分配（P75/P52/N121 族，历史已试 P226/P248 档②）⇒ 维持
//   （新=DIFF 125/130 gap=-5, blocks=7）。
// [P8S1 2026-09-11 本波复核] 维持封顶：求值序(ORIG mov -0xc 先于 movl $0xa)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
void CAssaultPlace::_ProcessChaos(int killer, int victim)
{
    if (victim < 0 || victim > 7)
        return;
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebbd2 `cmp %edx,%eax; sete %al;
    //   test %al,%al; je <函数尾>`（零槽物化）⇒ P226 裸语句表达式条件向。
    //   ORIG 0x082ebc25 `movzbl 0x128(%edx,%eax,1),%eax; xor $1; test %al,%al; je`
    //   ⇒ P248 档② 字段 bool 化 + 裸 if(!field)，无 chaosFlag 槽。
    // 操作数序：交换成 `field == call` 使 4.4 把字段提前载入 %ebx（push %ebx
    //   劣化，实测回退）；残差 = ORIG `cmp %edx,%eax` vs 我方 `cmp %eax,%edx`
    //   角色互换（N7/N18a 族）。
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebbd2 `call GetTeam; mov 0x8(%ebp),%edx;
    //   mov 0x124(%edx),%edx; cmp %edx,%eax` ⇒ 删 `(int)` cast（同 EvadeAssaultPlace）。
    if (({ m_assaulters[victim].GetTeam() == m_winTeam /*+0x124*/; })) {
        CUserCharacInfo* killerUser =
            (CUserCharacInfo*)m_assaulters[killer].GetUser();
        CUserCharacInfo* victimUser = (CUserCharacInfo*)m_assaulters[victim].GetUser();
        if (victimUser != 0) {
            if (!m_chaosFlags[m_assaulters[killer].GetTeam()]) {
                victimUser->AddCurCharacChaosPoint(10);
                if (victimUser->getCurCharacR() != 0) {
                    PacketGuard packet;
                    packet.put_header(0, 2);
                    packet.put_byte(0);
                    packet.put_short(1);
                    ((CUser*)victimUser)->make_basic_info((char*)&packet, 0);
                    packet.finalize(true);
                    G_GameWorld()->send_all(packet, (CUser*)victimUser);
                }
            }
            if (killerUser != 0) {
                // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebd32 `setg %al; test;
                //   je <0x082ebd46 Kill 块>` + then 直落 `jmp <0x082ebd51 Die 块>`
                //   ⇒ 原码是 `if (point > 0) Mode; else Kill;`（写 `< 1` 产 setle 反形）。
                if (killerUser->GetCurCharacChaosPoint() > 0)
                    victimUser->IncCurCharacChaosModeCount();
                else
                    victimUser->IncCurCharacChaosKillCount();
                killerUser->IncCurCharacChaosDieCount();
            }
        }
    }
}