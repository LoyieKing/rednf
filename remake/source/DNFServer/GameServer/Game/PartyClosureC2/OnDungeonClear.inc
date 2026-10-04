// ===========================================================================
// CParty::OnDungeonClear —— T 0x085b5518
// 报告：OnDungeonClear.md。统计上报、个人结算回调、历史日志、战场清场、
// 血腥统计包、村庄怪物战败幽灵惩罚。
// ===========================================================================
int CParty::OnDungeonClear(CUser* user, bool success)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [b28d-solo-PartyClosureC2] ORIG 0x085b5518：
    //   P225 推广嵌套 if 复现 `call; test; setne; test; je` 物化；
    //   (signed char)get_dimension_possible() 复现 ORIG `test %al,%al; jle`
    //   （头声明 unsigned char，同 CParty_KillChain 先例）；
    //   P66 左操作数先物化（std-5 <= level）＋ P52 单语句链式血腥包发送；
    //   penalty 非 const 防 4.4 折叠 penalty+600（ORIG 走 -0x10 槽计算）。
    // ---- 结算角色信息广播 ----
    if (IsCollectDungeonStatistic() && m_battleField.m_bloodState != 1)
    {
        if (user != 0)
        {
            if (user->getCurCharacR() != 0)
            {
                if (user->m_hasCharacterStatistic != 0 &&
                    m_battleField.m_pDungeon != 0)   // 旧 m_character.m_f40（+0x40）——布局修正后为 CUser 直属 +0x8e074
                {
                    SendDungeonCharacInfo(user, success);
                }
            }
        }
    }

    // ---- 经验统计（维度票充足 或 等级在标准±窗口内） ----
    if (m_battleField.m_bloodState == 1)
    {
        // skip
    }
    else if (m_battleField.m_pDungeon != 0 &&
             (signed char)m_battleField.m_pDungeon->get_dimension_possible() > 0)
    {
        GlobalData::s_statisticMgr->m_dungeonExp.IncreaseDungeonExp(user->get_charac_level(),
                                              user->m_character.m_f28,
                                              user->m_character.m_f30);
    }
    else if (m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->get_standard_level() - 5 <= user->get_charac_level() &&
            m_battleField.m_pDungeon->get_standard_level() + 3 >= user->get_charac_level())
        {
            GlobalData::s_statisticMgr->m_dungeonExp.IncreaseDungeonExp(user->get_charac_level(),
                                                  user->m_character.m_f28,
                                                  user->m_character.m_f30);
        }
    }

    // ---- 个人结算回调与历史日志 ----
    user->onDungeonClear(success);
    user->setDungeonClearHackCheck(success);
    user->m_historyLog.DungeonClearInfo(success ? 1 : 0,
                                        GlobalData::s_systemTime_.getCurSec() -
                                            m_field185c);

    bf(this).onClearMap(false);

    // ---- 血腥副本统计包（0x1036/0x14） ----
    // ---- 血腥副本统计包（Packet_Blood_dungeon_statistic ctor W 0x085bee52） ----
    if (user != 0 && m_battleField.m_pDungeon != 0)
    {
        if (user->getCurCharacR() != 0 &&
            static_cast<signed char>(m_battleField.m_pDungeon->m_bloodDungeonType) > 0)
        {
            Packet_Blood_dungeon_statistic stat;
            stat.m_level = user->get_charac_level();    // +0x0e
            stat.m_success = success ? 1 : 0;           // +0x13
            stat.m_one = 1;                             // +0x12

            GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                user->GetServerGroup())
                ->SendPacket(reinterpret_cast<char*>(&stat), 0x14);
        }
    }

    // ---- 村庄怪物战败：混乱/幽灵惩罚 ----
    if (bf(this).m_villageMonsterId != 0 && !success)
    {
        int penalty = -0x23a;                             // -570（非 const：ORIG 走槽计算）
        const int now = GlobalData::s_systemTime_.getCurSec();
        user->SetCurCharacChaosDieTime(now);
        user->SetCurCharacChaosResponTime(now + 600 + penalty);
        // ORIG 以静态派发调用（函数体不使用 this，mangled 与普通成员一致）
        pvp_assault::CAssaultMgr::NotifyGhostTime(user, penalty + 600,
                                                  penalty + 600);
        user->SetCurCharacGhost(true);
        user->m_characterView.enableSaveCharacView();
    }
    return 1;                       // ORIG 尾 mov $0x1,%eax
}