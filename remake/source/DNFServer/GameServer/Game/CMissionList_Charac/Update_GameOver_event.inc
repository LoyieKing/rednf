void CMissionList_Charac::Update_GameOver_event(
    CUser& user, const MissionClearCondition_Parameter& param)
{
    // b28c-L5：ORIG 0x085e5fe4 两守卫均为「call 结果寄存器直比 + bool 物化岛」：
    //   `call GetChannelType; cmp $0xf,%eax; setne %al; test %al,%al; jne <尾岛>`、
    //   `call getIndex_byKind; test %ax,%ax; setne %al; test %al,%al; jne <尾岛>`，
    // 且两失败出口落**不同** nop 中继（+0x1b5 / +0x1b8），body 后另有 `jmp`。
    // 4.4 对正极性 `if (a==K && b==0) {body}` 恒产 je/jne 直跳 + 结果汇聚岛
    // （少 setne、多 mov$1/jmp/mov$0），探针 /tmp/l5_probe_a.cpp hA
    // （`if (g1(x) != 0xf) return;`）实证否定早返直产 ORIG setne 三段物化；
    // 双岛由 goto 尾标签串复现（P223/BM8）。std::vector 局部包内层块，
    // 避开 4.4「goto 跨同层非平凡局部声明」拒编译约束。
    if (G_GameWorld()->GetChannelType() != 0xf)
        goto L_notchannel;
    if (getIndex_byKind(0x1b) != 0)
        goto L_hasmision;
    {
        alter();
        std::vector<int> out;
        check_mission_kind(1, user, param, out);
        check_mission_kind(3, user, param, out);
        check_mission_kind(8, user, param, out);
        check_mission_kind(0xd, user, param, out);
        check_mission_kind(0x10, user, param, out);
        check_mission_kind(0x12, user, param, out);
        check_mission_kind(0x17, user, param, out);
        Send_Cleared_Mission_List(user, out);
    }
L_hasmision:;
L_notchannel:;
