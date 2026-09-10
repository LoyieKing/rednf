void CAssaulter::GiveupPanalty()
{
    // b28d-solo-CAssaultMgr: AE — 尾块由「空 then + else body」改单出口正向
    // 包裹 `if (ghostTime != 0)`（消 4.4 尾 `jmp <epilogue>; nop` +2），
    // 144→142 与 ORIG 142 逐条 AE。
    // b28d-L3: DIFF/N7+N82 — ORIG 0x82e6694.. chaos/level 实参内联后帧 0x24 vs ORIG 0x20、缺 push %esi（ORIG 值留 %esi 跨调用）
    // P183/P157：ORIG 不缓存 m_user（每点 `mov this; mov (%eax)` 重装载，
    // penalty/now 处才用 %esi/%ebx 跨调用缓存）——重建禁 named user。
    if (m_user != 0)
    {
    // ORIG +0x1d：getCurCharacR `test %eax; setne %al; test %al; je`——4.4 对
    // 裸 `!= 0` 指针比较恒产同款两段物化（带变量赋值反改 slot store+cmpl）。
    if (m_user->getCurCharacR() != 0)
    {
        m_user->setCurCharacBackupStamina(m_user->getCurCharacStamina());
        // ORIG 0x082e6868（P48/P84）：两处独立 IsPowerWarEventOn call、$1/$0
        // 物化岛 + 二次 test 汇聚。第一段真值（+0x5d `xor $0x1` 后 test;je→$0
        // 岛，+0x73 `cmp $7; je→$0`）= `!pw && vill!=7` → GiveupPvP(7)，
        // else→(3)（两分支独立 call）；第二段（+0xd1 `test; jne→$1`，
        // +0xe2 `cmp $7; jne→$0`）= `pw || vill==7`。
        // N96 清绑：ORIG $1/$0 物化岛由合并短路条件自然产生（P133）。
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            m_user->getCurCharacVill() != 7)
        {
            m_user->m_historyLog.GiveupPvP((ENUM_PVP_HISTORY_REASON)7);
        }
        else
        {
            m_user->m_historyLog.GiveupPvP((ENUM_PVP_HISTORY_REASON)3);
        }
        int ghostTime = 0;                    // ORIG +0xbb 第二段前预清零
        if (GlobalData::s_power_manager->IsPowerWarEventOn() ||
            m_user->getCurCharacVill() == 7)
        {
            float penalty = GlobalData::s_power_manager->GetPowerWarResponPenalty(m_user);
            ghostTime = GlobalData::s_power_manager->GetPowerWarGhostTime(m_user, penalty);
        }
        else
        {
            // b28d-L3：ORIG 0x82e6a?? 先求第2实参 GetCurCharacChaosPoint→%esi
            // 跨 get_charac_level call 保活（无槽往返 P0）⇒ chaos/level 与
            // receiver（CDataManager::m_serverParameter 即 +0x68）全部内联为
            // 实参/receiver 表达式，消具名 sps 槽（帧 0x24→0x20 对齐 ORIG）。
            ghostTime = G_CDataManager()->m_serverParameter.getAssaultGhostTime(
                m_user->get_charac_level(), m_user->GetCurCharacChaosPoint());
        }
        // b28d-solo-CAssaultMgr: ORIG 0x82e69d0 `cmpl $0,-0x14; je <epilogue>`
        // + body 直落 epilogue（无尾 jmp/nop）。试单出口包裹形。
        if (ghostTime != 0)
        {
        m_user->SetCurCharacGhost(true);
            //（ORIG 用 %ebx callee-saved 缓存 now；4.4 绑 %ebx 实测反增 spill，
            // 维持具名槽，寄存器选择差记 N7/N82。）
            int now = GlobalData::s_systemTime_.getCurSec();
            m_user->SetCurCharacChaosDieTime(now);
            m_user->SetCurCharacChaosResponTime(now + ghostTime);
            m_user->m_characterView.enableSaveCharacView();
        }
    }
        }
