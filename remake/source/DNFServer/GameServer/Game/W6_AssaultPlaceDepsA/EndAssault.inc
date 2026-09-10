void CAssaultPlace::EndAssault()
{
    // [b28d-solo-W6_AssaultPlaceDepsA] P309 试验：ORIG 槽 i=-0x10 / allDead=-0x9
    //   （4.1 先声明深）⇒ 头部声明 i、allDead（赋值留原位）试归位。
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
