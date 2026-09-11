// ORIG 0x081c4d26 T _ZN18DisPatcher_MoveMap26CheckOneMapPlayTimeHackCntEP5CUserR12MSG_MOVE_MAP
// 以队伍地图游玩时长（死亡塔为 stage 通关时限/1000+5）为基准，逐成员修正
// msg.m_23[i] 上报 tick：>10000 部分按万位上报（0x2afe 副本记 0x82 否则 0x77）；
// job3 / job4 觉醒2 减半 ×0.5 否则 ×0.8；未开局且 >limit 时按 t<=1.5*limit
// [b28d-29] N82/P183：帧 0x64 vs 0x94 + 槽平移，ORIG 无缓存局部而 4.4 新增；语义集合全等  (ORIG 0x081c4d26 insn 412->366)
// 单次、超幅 ×10 次记 0x74/0x81。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 516（ORIG 411 / ours 363），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] P167 差集 5 缺→0 缺（CALLSITE CLEAR，39/39）：ORIG 0x081c4e82/
// 0x081c4eda/0x081c50e3/0x081c5145/0x081c520a/0x081c526c 六处判定各自重新 call
// CUser::GetParty 并直读 +0xcac（不设 dungeon 局部）；0x081c4f83/0x081c4f93 两次
// get_charac_job；0x081c505a 的 check_start_point 用函数头 user party 缓存（-0x24）
// 而非 member 的 party。改后 ours 363→377（ORIG 411）。
// ⚠️ 本件 ndiff 370→398（+28）：ORIG `==0` 带 sete 物化，4.4 O0 恒折直分支（N78/
// setcc-fold 族）⇒ 形态代价，语义正确性优先故保留；回滚只需撤销本函数三处展开。
// [SweepDispatchCore 2026-09-09] P183 修复：删 `unsigned int& slot = info.m_23[idx]`
// 与 `CBattle_Field& battleField` 两处引用缓存，全部改 `info.m_23[idx]` / 内联取址
// 表达式（ORIG 13 处每次重算 `mov idx; add $8; mov 0x3(base,idx,4)`，实证 0x081c4e0a/
// 0x4e30/0x4f6d/0x4fbc/0x5075/0x50dd/0x50f7/0x5164/0x51a9/0x521e/0x528b/0x52d0）；
// halfTick 具名 bool 改 if/else 表达式（ORIG 无 bool 槽：0x4fac `mov $1` / 0x4fb3
// `mov $0` 岛直 test）。gap 32→-10（ours 380→422，ORIG 412），p167 CLEAR（39/39），
// 四数不回退（213/639/2/74）。
void DisPatcher_MoveMap::CheckOneMapPlayTimeHackCnt(CUser* user, MSG_MOVE_MAP& info)

{
    // [DpcMisc-R2 2026-09-11 负结论·16 件清单内] 本件 ORIG 412 / OUR 418（+6）。
    // 逐条归一化后残差四类，全部为跨版本后端/旗标族（源码形无自由度），维持原形：
    //  ① +2：limit 除法段中间物化 `mov %eax,-0x10(%ebp); mov -0x10(%ebp),%eax`
    //     （ORIG 0x081c4d8b 处 `shr $0xd,%eax; imul $0x2710,%eax,%eax` 直连，
    //     4.4 插一次槽往返；属 4.4 调度，非源侧具名局部——该表达式已是单条语句）。
    //  ② -3：ORIG 把 addServerHackCnt 的第 4 实参先物化 `mov -0x14(%ebp),%ebx` 再
    //     `mov %ebx,0x10(%esp)`（callee-saved 缓存），4.4 改 `mov -0x14(%ebp),%edx;
    //     mov %edx,0x10(%esp)`（scratch %edx）⇒ 同条数、寄存器角色差（N7 族）。
    //  ③ 浮点比较 lowering：ORIG `fucompp; fnstsw %ax; test $0x45,%ah; sete %al`
    //     vs 4.4 `fucomip %st(1),%st; fstp %st(0); seta %al`（P230：需 -march=i486
    //     才消 fnstsw/sahf 族；TU 级旗标由 dnf_game_cxx.sh 定，本泳道不可改）。
    //  ④ +4：函数尾/循环尾各一组 4.4 自产 `jmp <epilogue>; nop` 中继岛（N121）。
    // p167：父泳道实测本件 C1/C2·D1/D2 同址别名外零差集 ⇒ 无语义缺口。
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
