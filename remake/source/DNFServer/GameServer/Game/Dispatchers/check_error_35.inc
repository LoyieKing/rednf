// b28d-solo-Dispatchers verdict=DIFF(routed) [S206 收敛 9→1 块] ORIG 0x0825eec4
//   [N82→P291/P311] 已修：+0xd 区改具名数组成员视图（scaled 索引 `mov 0xd(%eax,%edx,4)`）
//   + find_quest 结果具名局部落槽比较；残余仅 N18a 单段 regmove（ORIG
//   `mov 0xd(%eax,%edx,4),%eax; mov %eax,%ebx` vs 4.4 折 `…,%ebx`，P254 enum 化未过）。
int Dispatcher_SaveQuestNotify::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x0825eec4：user/当前角色空 → -1；state<=2 → -2；
    // 扫 msg+0xd 起 10 槽 int，非 0 且 find_quest 失败 → -3；否则 0。
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -2;
    }
    // [b28d-solo-S206] P291/P311：ORIG 0x0825efxx 对 +0xd 区是**scaled 索引**直读
    //   （`mov -0x10(%ebp),%edx; mov -0x14(%ebp),%eax; mov 0xd(%eax,%edx,4),%eax`
    //   两用点各重算，P48/P157）；`reinterpret_cast<const int*>(m_0d+i*4)` 恒折
    //   add/shl/lea 且多 -0xc 具名槽 ⇒ 改具名数组成员视图（PacketTypes.h 冻结，
    //   m_0d 为 char[0x28]，同偏移同宽度，sizeof/布局零变更）。
    struct __attribute__((packed)) QuestNotifyView
    {
        char m_head[0xd];
        // [b28d-solo-Dispatchers] P211 档③/P254：ORIG 0x0825ef36 第二用点对
        //   `mov 0xd(%eax,%edx,4),%eax; mov %eax,%ebx`（字段→eax→callee-saved）
        //   为「字段与形参异型」的转换中转形；视图字段 unsigned + find_quest(int)
        //   ⇒ 4.4.6 复现两段（探针 /tmp/probe_disp.cpp t3 实证）。宽度/偏移不变。
        unsigned int m_questIdx[10];       // +0x0d..+0x34
    };
    const QuestNotifyView& req = reinterpret_cast<const QuestNotifyView&>(msg);
    for (int i = 0; i <= 9; ++i)
    {
        if (req.m_questIdx[i] != 0)
        {
            // [b28d-solo-S206] ORIG 0x0825efxx：find_quest 结果落 -0xc 具名局部后
            //   `cmpl $0x0,-0xc(%ebp); jne <next>`（无 bool 物化岛）。
            Quest* found = G_CDataManager()->find_quest(req.m_questIdx[i]);
            if (found == 0)
            {
                return -3;
            }
        }
    }
    return 0;
}