// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82 族），ORIG 0x086388xx 见 report。
// [b28d-solo-TimerDispatcher] 判词 setcc-fold/N82 封顶：vector::size() 比较 ORIG
// 直跳 `cmp;jg` vs 4.4 `setl;test;je` 物化（P197④ 族）（280/279）。
// [TimerDispatcher-2 2026-09-09] 修 x87 因子结合序（ORIG 0863883f..08638856 /
// 0863886e..08638885 实证：(count*ratio)*(CP2/CP1)*(ratio+1.0)）⇒ ndiff 64→36；
// 残余 = x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp，N85/P230，
// 需 -march=i486 路由，已报 Main）+ clamp 段 setl 物化（P197④；S1 已试 `<=` 形回滚）。
bool TimerGeneratePowerUp::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 08638860：`call IsPowerWarEventOn; test %al,%al; je +0x3f4(共享尾 mov $1)`
    // ⇒ 正向 if 包裹 + 共享尾 return 1（P222/P214①）；旧码 `if(!f()) return 1;` 产
    // xor$1 + 局部 mov$1/jmp 岛（多 3 条）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn())
    {
        GlobalData::s_power_manager->SetPowerUpRemainTime(
            G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
        GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d, 5, 0, 0);
        registNextTimer(G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
        G_GameWorld()->CancelPowerUp();
        m_team1Users.clear();
        m_team2Users.clear();

        const double ratio =
            static_cast<double>(G_CDataManager()->m_powerParameter.m_powerUpRatio) / 100.0;  // /_DAT_08ce8590(=100.0)
        const int team1Count = GlobalData::s_power_manager->GetPowerUserCount(1);
        const int team2Count = GlobalData::s_power_manager->GetPowerUserCount(2);
        const int team1ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(1);
        const int team2ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(2);
        const int team1Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)1);
        const int team2Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)2);
        const int mod = G_CDataManager()->m_powerParameter.m_powerUpPointMod;  // dm+0xa6d0
        if (mod == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0xff4, 0);
            trace("[PowerWar] PowerUpMod is 0");
            sendPowerUpInfo(0, 0);
            return 1;
        }

        double team1Ratio = static_cast<double>(team2Point - team1Point) / mod;  // local_24
        double team2Ratio = static_cast<double>(team1Point - team2Point) / mod;  // local_1c
        if (0.9 < team1Ratio)
            team1Ratio = 0.9;
        if (team1Ratio < -0.9)
            team1Ratio = -0.9;
        if (0.9 < team2Ratio)
            team2Ratio = 0.9;
        if (team2Ratio < -0.9)
            team2Ratio = -0.9;
        if (team1ChannelPoint == 0 || team2ChannelPoint == 0)
        {
            sendPowerUpInfo(0, 0);
            return 1;
        }

        // [fix] ORIG 0863883f..08638856 / 0863886e..08638885 因子序实证：
        //   fildl team2Count(-0x38) → fmull ratio(-0x48) → fildl team2CP(-0x30)
        //   → fildl team1CP(-0x34) → fdivrp → fmulp → fldl team1Ratio(-0x20) → fld1
        //   → faddp → fmulp ⇒ 原码 = (count*ratio)*(CP2/CP1)*(ratio+1.0)。
        // 浮点乘不可结合，结合序改变可观察结果 ⇒ 旧码 (ratio+1.0) 起首（Ghidra
        // 槽号错位误抄）是真语义差，按 ORIG 因子序还原。
        int team1PowerUp = static_cast<int>(team2Count * ratio *
                                            (static_cast<double>(team2ChannelPoint) / team1ChannelPoint) *
                                            (team1Ratio + 1.0));
        int team2PowerUp = static_cast<int>(team1Count * ratio *
                                            (static_cast<double>(team1ChannelPoint) / team2ChannelPoint) *
                                            (team2Ratio + 1.0));
        G_GameWorld()->GetPowerUpDomain(m_team1Users, m_team2Users);
        // [TimerDispatcher-3 判据①] 旧 if-赋值形在 4.4 恒产 `setl;test;je` 物化
        // （3 条），ORIG 是 `cmp; jg <保持臂>; [二次 size()]; jmp <汇合>;
        // 保持臂: mov x→%eax; 汇合: mov %eax→x`（clamp 直跳 + 中继 jmp + 自赋值对，
        // 探针 /tmp/td_probe4.cpp g6/g7 逐条同构）⇒ 原码为**三目赋值**。
        // 语义等价（size>x 保持 x，否则取 size()），二次 size() 调用位置亦吻合。
        team1PowerUp = (static_cast<int>(m_team1Users.size()) > team1PowerUp)
                           ? team1PowerUp
                           : static_cast<int>(m_team1Users.size());
        team2PowerUp = (static_cast<int>(m_team2Users.size()) > team2PowerUp)
                           ? team2PowerUp
                           : static_cast<int>(m_team2Users.size());
        generatePowerUp(m_team1Users, team1PowerUp);
        generatePowerUp(m_team2Users, team2PowerUp);
        sendPowerUpInfo(team1PowerUp, team2PowerUp);
    }
    return 1;
}