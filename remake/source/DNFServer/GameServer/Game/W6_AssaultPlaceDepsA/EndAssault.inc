// ---------------------------------------------------------------------------
// ORIG T 0x82e99a2 (size 0x133)：EndAssault() —— 单播结算包 + 清场计时
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] NEAR 封 N（P321-T5）：P223 双标签 goto 消循环
//   nop 岛 + 头部声明 i/allDead 使 i=-0x10/allDead=-0x9 归位后 88=88；残差
//   N82附——4.4 聚合对象优先占低槽：PacketGuard -0x24 vs ORIG -0x1c、
//   aliveCounts -0x18/-0x14 vs ORIG -0x24/-0x20。P309 头部声明 aliveCounts
//   （无初始化+后置赋值）实测无效（4.4 仍给块内 PacketGuard 低槽），已回退。
//   p167 CLEAR。ORIG 0x082e99a2
void CAssaultPlace::EndAssault()
{
    // [b28d-solo-W6_AssaultPlaceDepsA] P309 试验：ORIG 槽 i=-0x10 / allDead=-0x9
    //   （4.1 先声明深）⇒ 头部声明 i、allDead（赋值留原位）试归位。
    // [L2-S5 实验-D 负结论] 把 aliveCounts 提到函数头（i/allDead 之前、声明序
    //   深→浅）⇒ 4.4 仍给**块内聚合** PacketGuard 低槽 -0x24、counts 停 -0x18
    //   （槽图逐条不变、四数不变）⇒ 证实 N82附「块内聚合优先占低槽」固定分区，
    //   声明序不可翻，已回滚。
    int i;
    bool allDead;
    for (i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e99c9 `test %al,%al; jne +0xad`
        //   （nop 岛）+ 体尾 `jmp +0xae` ⇒ P223 双标签 goto 形（if{}else{} 被 4.4
        //   jump-thread，少 jmp+nop 两条）。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)7);
            PacketGuard packet;
            _MakeEndAssault(&packet);
            ((CUser*)m_assaulters[i].GetUser())->Send(packet);
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }

    // [C07] N82附 封顶：aliveCounts 与块内 PacketGuard 槽深浅互换（ORIG counts
    //   -0x24/-0x20 < packet -0x1c；OURS packet -0x24、counts -0x18/-0x14）。
    //   P309 两形实测（头部声明+后置赋值 / 提到 i 之前）槽图均不变 ⇒ 4.4
    //   「块内聚合优先占低槽」固定分区，不可翻。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N82附）→ 试=①prett2/修正判据缺口 0、
//   strall 串差 0、p167 CLEAR；②446 后端与 446h 变体四数逐项相同（本 TU 未路由，
//   两向变体均无增益）；③本波复核 88=88 纯槽号差（packet -0x24 vs ORIG -0x1c、
//   aliveCounts -0x18/-0x14 vs ORIG -0x24/-0x20），P339 声明序两形已试不可翻 ⇒
//   维持（新=NEAR 88/88 gap=0, blocks=9）。
    int aliveCounts[2] = { 0, 0 };
    // [b28d-62] P232：ORIG 0x082e9a71 有 dead bool 预清 `movb $0x0,-0x9`（后续
    //   无读取），缺失导致 i 槽 -0xc vs ORIG -0x10 全帧浅 4 ⇒ 按原样补声明。
    allDead = false;
    (void)allDead;
    _GetTeamCountAliveUser(aliveCounts);  // -0x24 起（两连 int）
    if (aliveCounts[0] == 0)
        m_teamFlag[1] = 1;                // this+0x121
    if (aliveCounts[1] == 0)
        m_teamFlag[0] = 1;                // this+0x120

    TimerCloseAssault::registNextTimer(
        5, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);
}