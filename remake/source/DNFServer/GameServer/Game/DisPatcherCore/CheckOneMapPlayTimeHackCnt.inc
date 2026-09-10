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
            // [DpcMisc-C1] P0/P329 删具名临时：ORIG 0x081c4d83 `call getStageClearTime`
            // 后仅 0x081c4d88 一条 `mov %eax,-0x3c(%ebp)`，紧接 0x081c4d8b
            // `mov -0x3c(%ebp),%eax; mov $0x10624dd3,%edx; mul %edx`（该 -0x3c 即除法
            // 被除数临时槽，与 else 分支 0x081c4dad 共用）⇒ 原码在 limit 表达式处内联。
            // 改前我方 `mov %eax,-0x20; mov -0x20,%eax; mov %eax,-0x4c`（具名槽+复制），
            // 4.4.6 探针实测 具名形 6 条 / 内联形 4 条 ⇒ 预期 our −2，且帧 $0x74→$0x64、
            // tower/party/limit 槽 -0x24/-0x28/-0x2c → ORIG -0x20/-0x24/-0x28 全对齐。
            limit = (unsigned int)tower->getCStage()->getStageClearTime() / 1000 + 5;
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
        // [DpcMisc-C2] P59/P222 拆短路物化：ORIG 0x081c5065..0x081c5085 为两次
        // **独立直跳出**——`call check_start_point; xor $0x1,%eax; test %al,%al;
        // je 0x081c5316` 与 `mov 0x3(...); cmp -0x28(%ebp),%eax; jbe 0x081c5316`，
        // 无 bool 岛；改前我方 `!A && B` 在 4.4.6 下物化
        // `mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al; je`（.o 1971f-1972d）。
        // 4.4.6 探针实测：`if(!A && B)` 含 4 条岛 / 嵌套 if 无岛 ⇒ 预期 our −5。
        // 内层 if 与块体缩进保持原样（不重排 200 行），仅拆条件行。
        if (!reinterpret_cast<CPartyBattleFieldView*>(party)
                 ->m_battleField.check_start_point())
        if (info.m_23[idx] > limit)
        {
            // [DpcMisc-C3] 分支极性/块序还原（P59/P222 + 语义直证）：ORIG 0x081c50bf
            // `fucompp; fnstsw %ax; test $0x45,%ah; sete %al; test %al,%al; je 081c5204`
            // —— fucompp 比较 st0=(double)val / st1=1.5*limit，「大于」时 C3C2C0=0
            // ⇒ sete 得 al=1 ⟺ val > 1.5*limit，而 je 在 al==0（val ≤ 1.5*limit）时
            // 跳到 0x081c5204（**单次**记录块，0x081c5204-0x5313）；fall-through
            // 0x081c50d1 才是 10 次循环块。⇒ 原码为 `if (val > 1.5*limit) {循环} else {单次}`。
            // 改前我方写作 `if (val <= 1.5*limit) {单次} else {循环}`（4.4.6 遂把单次块
            // 放 fall-through、循环块跳转，两块 40+ 行物理倒置 ⇒ ndiff 大头）。
            // 4.4.6 探针实证（probe_tmp p1/p2）：块序严格随源码 then/else 书写序
            // ⇒ 本改写可对齐块序；代价 = 比较段多 1 条 `fxch %st(1)`（seta 形）。
            if ((double)info.m_23[idx] > 1.5 * (double)limit)    // 0x8bd6330=1.5
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
            else
            {   // 单次记录（ORIG 0x081c5204 块）
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
        }
    }
}
