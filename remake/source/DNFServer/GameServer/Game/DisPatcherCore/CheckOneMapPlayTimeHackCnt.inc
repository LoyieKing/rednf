void DisPatcher_MoveMap::CheckOneMapPlayTimeHackCnt(CUser* user, MSG_MOVE_MAP& info)

{
    unsigned int limit = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    if (user->get_state() == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            unsigned int clearTime =
                (unsigned int)tower->getCStage()->getStageClearTime();
            limit = clearTime / 1000 + 5;
        }
    }
    else
    {
        limit = party->GetMapPlayingTime() / 1000;
    }
    for (int idx = 0; idx < 4; ++idx)
    {
        if (!party->checkValidUser(idx))
        {
            continue;
        }
        CUser* member = party->get_user(idx);
        if (member == 0)
        {
            continue;
        }
        if (info.m_23[idx] > 10000)
        {
            // [b28e-L5] 判据⑫（clr_l orig=5 / our=3）：ORIG 0x081c4e22 与 0x081c4e29
            // 各有一条 `movl $0x0,-0x14(%ebp)` / `movl $0x0,-0x10(%ebp)`（两个具名
            // int 局部**预清**），随后 0x081c4e4f/0x081c4e79 才回写商与余数 ⇒ 原码是
            // 「声明=0 + 赋值语句」两句式（P198① 预清必写 / 判据⑫ 缺具名局部），
            // 初始化式会被 4.4 折成单条 store（我方案发 2 条预清）。
            unsigned int quotient = 0;
            unsigned int remainder = 0;
            quotient = info.m_23[idx] / 10000;
            remainder = info.m_23[idx] % 10000;
            // ORIG 不为 dungeon 设局部：0x081c4e82（==0 判定）与 0x081c4eda
            // （else-if 取 index）各自重新 call CUser::GetParty 并直读 +0xcac。
            if (static_cast<CParty*>(member->GetParty())->m_battleField.m_pDungeon == 0)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            else if (static_cast<CParty*>(member->GetParty())
                         ->m_battleField.m_pDungeon->get_index() == 0x2afe)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x82, 1, quotient, 0);
            }
            else
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            info.m_23[idx] = remainder;
        }
        // ORIG 0x081c4f83 / 0x081c4f93 两处各自 call get_charac_job，结果经
        // `mov $1 / $0` 岛物化后直接 test（无 bool 槽）⇒ 表达式直用于 if/else。
        // ORIG .rodata：0x8bd6320=0.5 / 0x8bd6328=0.8（fildll×const 后 fistpll 截断）
        if (member->get_charac_job() == 3 ||
            (member->get_charac_job() == 4 &&
             member->getCurCharFirstGrowType() == 2))
        {
            info.m_23[idx] = (unsigned int)((double)info.m_23[idx] * 0.5);
        }
        else
        {
            info.m_23[idx] = (unsigned int)((double)info.m_23[idx] * 0.8);
        }

        // ORIG 0x081c505a 用 -0x24(%ebp)（= 函数头 user->GetParty() 的缓存）+0xb24，
        // 不另取 member 的 party；P183：ORIG 单次取址，无引用缓存槽。
        if (!reinterpret_cast<CPartyBattleFieldView*>(party)
                 ->m_battleField.check_start_point() &&
            info.m_23[idx] > limit)
        {
            if ((double)info.m_23[idx] <= 1.5 * (double)limit)   // 0x8bd6330=1.5
            {   // 单次记录
                // ORIG 0x081c50e3 / 0x081c5145：两判定各自重新 call GetParty。
                if (static_cast<CParty*>(member->GetParty())
                        ->m_battleField.m_pDungeon == 0)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                }
                else if (static_cast<CParty*>(member->GetParty())
                             ->m_battleField.m_pDungeon->get_index() == 0x2afe)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x81, 1, info.m_23[idx], 0);
                }
                else
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                }
            }
            else
            {   // 超幅：同一记录重复 ×10（ORIG 循环 10 次，无 break）
                for (int rep = 0; rep < 10; ++rep)
                {
                    // ORIG 0x081c520a / 0x081c526c：同上，逐次重新取 party。
                    if (static_cast<CParty*>(member->GetParty())
                            ->m_battleField.m_pDungeon == 0)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                    }
                    else if (static_cast<CParty*>(member->GetParty())
                                 ->m_battleField.m_pDungeon->get_index() == 0x2afe)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x81, 1, info.m_23[idx], 0);
                    }
                    else
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                    }
                }
            }
        }
    }
}
