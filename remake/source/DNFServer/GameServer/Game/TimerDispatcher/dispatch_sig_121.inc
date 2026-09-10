int Timer_UpdateOnlinePreliminaryTeam::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08639700：FindTeam/RegistNextTimer 均用 p2（0x10(%ebp)），非 p3。
    (void)p1;
    (void)p3;
    online_preliminary::COnlinePreliminaryTeam* team =
        GlobalData::s_onlinePreliminaryTeamMgr->FindTeam(p2);
    // ORIG 0x08639722：team 判空 je +0x4e；registered 为 bool 局部（槽 -0x9）
    // 经 `movzbl; xor $1; test; je +0x55` 判定；两块 `mov $0x0,%eax` 岛 +
    // `mov $0x1,%eax` 尾岛。布局钥匙（P222 正向包裹）：正极性 if(team!=0) 包
    // body、return 0 归共享尾（je 跳过 body）；`if (registered) return 1;` 后
    // fall-through 到 return 0。goto 标签串形被 4.4 逐标签加 nop（+3 insn）。
    if (team != 0)
    {
        team->saveData();
        const bool registered = RegistNextTimer(p2);
        if (!registered)
            return 0;
    }
    else
    {
        return 0;
    }
    return 1;
