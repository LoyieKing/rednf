// ---------------------------------------------------------------------------
// CBattle_Field::BloodPhaseSpawnTimerResgister(short) —— ORIG T 0x083053d2：
//   为当前小轮注册各阶段定时器（0x17..0x20），疯狂轮额外发包。
// ---------------------------------------------------------------------------
int CBattle_Field::BloodPhaseSpawnTimerResgister(short smallRound)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x083053d2：
    // ①无 curRound/hi/tq 具名局部——smallRound 形参 spill 槽 `addw $1` 自增、
    //   long long 高半字由 %esi 承载、receiver 直接 G_TimerQueue()（P52 链式）；
    // ②`cnt==0` 写 `++cnt`（ORIG addl 形，P38）；
    // ③循环体用 smallRound 本身（非副本），`++smallRound` 在
    //   incBloodSmallRoundOrder 之前。
    // [L31 round2 · 判据⑩/⑪] ORIG 槽表：key@-0x34、cnt@-0x24、i@-0x20、interval@-0x1c
    // （ORIG 0x083054a6 `mov %eax,-0x34(%ebp)` 为循环内赋值点）⇒ key 用「无初值声明 +
    // 延后赋值」。两形实测：key 前置声明 **18→2 行差**（保留）；key 后置（cnt 之后）
    // 反劣 2→8 行（已回滚）。残余 2 行为 key 槽 -0x28 vs ORIG -0x34（0xc 错位，
    // P212/N93 块级槽方向族，无更优源形）⇒ 封 N。
    // [本轮 round5 实测补充] ORIG 尚有 -0x30 槽 = 第 4 实参 `i+0x17` 的具名物化
    // （0x083054b3-0x083054b9，跨 GetPartyIndex 保活，P0）；引入具名 int（头声明 +
    // 块内声明两形）实测恒把余槽整体加深 0x10（帧 0x4c→0x5c、键槽 -0x2c）⇒ 反劣，
    // 已回退。两件合计 NEAR(ndiff=4) 封顶（N82/N126 槽族）。
    // [round5 实验记录·第四形] key 声明后置（cnt 在前）实测：key 得 -0x24（方向对）
    // 但 cnt 被挤到 -0x28（原 -0x24 才对）⇒ 仅两槽互换、ndiff 4→16 反劣，已回滚。
    // 与 msg 形（整体加深 0x10）、while 无关；ORIG 六槽表（-0x34/-0x30/-0x2c/-0x24/
    // -0x20/-0x1c）在 4.4 五槽分配下不可同时满足 ⇒ N82/N126 封顶。
    int key;                                  // ORIG key（-0x34）
    int cnt = get_blood_spawn_cnt_of_small_round(smallRound);
    if (cnt < 0 || 10 < cnt)
        return -1;
    if (cnt == 0)
        ++cnt;
    for (int i = 0; i < cnt; ++i)
    {
        int interval = get_blood_spawn_small_round_interval(smallRound);
        if (interval < 1)
            return -1;
        if (m_bloodCrazyRound == smallRound)
            BloodCrazyMonsterSpawnPacket();
        m_bloodSpawnCnt[i] = smallRound;
        m_bloodSpawnTime[i] = 0;
        key = (int)m_party->gen_timer_key((TIMER_MESSAGE)(0x17 + i));
        // [b28d-round5 实测] ORIG 0x083054b3-0x083054b9 `mov -0x20(%ebp),%eax;
        // add $0x17,%eax; mov %eax,-0x30(%ebp)` 是第 4 实参的具名物化槽，但本工具链
        // 引入具名 int（头声明或块内声明两形实测）恒把余槽整体加深 0x10（0x4c→0x5c，
        // 键槽 -0x2c vs ORIG -0x34）⇒ 反劣，回退为内联实参（残余仅 key 槽 -0x28 vs -0x34）。
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_party->GetPartyIndex(),
                                                (TIMER_MESSAGE)(0x17 + i),
                                                (long long)interval,
                                                key, 0);
        ++smallRound;
        m_bloodRound.incBloodSmallRoundOrder();
    }
    return 0;
}