void CDeathTower::CPlayData::generateRewardItem(int memberIdx, int count,
                                                const CDungeonMgr& dungeonMgr,
                                                unsigned int param4, unsigned int param5,
                                                CParty* party)
{
    m_rewards[memberIdx].m_itemCnt = count;

    // ORIG：rate 值在各自使用点内联 G_CDataManager() 读取（循环内每次）
    for (int i = 0; i < count; ++i)
    {
        stGenerateRefData_t ref;
        stGenerateResult_t result;

        ::CLuckPoint luck;                  // sizeof 0x24（ORIG -0x88..-0x64 区；全局 CLuckPoint）
        if (party != 0)
            party->GetLuckPoint(&luck, false);

        const int roll = get_rand_int(1000);
        // ORIG 0846204b..0x097：`imul 0x1c(%ebp),%eax; cmp -0x20(%ebp),%eax;
        //   seta %al; test %al,%al; je <受控体>` —— 比较为 **unsigned**
        //   （prob0*param5 <= roll ⇒ 受控），且 boundHigh **不落槽**（P0：原码把
        //   表达式内联进条件，只调一次 G_CDataManager）。旧重建用 signed
        //   `(int)boundHigh` + 具名槽 + 二段比较（jl/jne），符号性与槽皆不符。
        // [A/B 轴复查 2026-09-10，**已试后回退**] ORIG 0846204f `cmp -0x20(%ebp),%eax; seta %al`
        //   为 mem 直比；改把 roll 写在比较右侧（`expr <= roll`）实测**无效** —— 4.4 仍先
        //   载 roll 入寄存器（形态 setae/setb 不变、our 357→359 劣化 +2）⇒ 按 N18b/P197④ 封顶。
        if ((unsigned int)roll >= (unsigned int)(
                G_CDataManager()->m_deathTower.m_rewardItemProb[0] * (int)param5))
        {
            // ---- 受控分支（ORIG +0x2df：vtable+0x08 random）----
            // ORIG 084622a4..0x2b6：prob1 只求值一次（`imul; cmp; seta; je`），
            //   条件 = prob1*param5 > roll（unsigned）⇒ 写单比较（旧重建写
            //   `<= && - != 0` 两次求值，多一次 G_CDataManager）。
            // 同上（A 轴/N18b 封顶）：ORIG 084622ae `cmp -0x20(%ebp),%eax; seta %al`；
            //   交换操作数同样不产生 mem 直比，已回退。
            if ((unsigned int)roll < (unsigned int)(
                    G_CDataManager()->m_deathTower.m_rewardItemProb[1] * (int)param5))
            {
                // ref 上下文区（ORIG 084622bc..328：movb/movb/movl/movl/movl）
                ref.m_field35 = 0;
                ref.m_field34 = (char)param4;
                ref.m_rankBonusIndex = 0x3f800000;             // float 1.0f 位型（ORIG movl）
                ref.m_dungeon = dungeonMgr.getDungeon();       // ORIG movl（指针）
                *reinterpret_cast<int*>(&ref.m_goldBase) = 0;  // ORIG movl $0 覆盖 +0x3c/+0x3e

                // [P282 真 bug 修] ORIG 08462311 `mov (%eax),%edx; add $0x8,%edx;
                //   mov (%edx),%ecx; call *%ecx` ⇒ 本分支（roll>=boundHigh、ref
                //   0x3f800000）走 **vtable+0x08**；ORIG _ZTV @0x08c9a788
                //   word[4]=0x0853a400=generateRandomItem ⇒ 原码本分支调
                //   generateRandomItem（旧重建调 generateSpecificItem=+0x0c，相反）。
                // [P0] ORIG 无 genMgr/gen 具名局部：084622f2..0846230d 为直链
                //   `call G_CDataManager; call getItemGenerator; …; mov (%eax),%edx`。
                G_CDataManager()->getItemGenerator()
                    ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                    ->generateRandomItem(ref, result);

                // [P0/配方 C05② 正向（A27 机理）] ORIG 无 dropEmpty 具名局部：
                //   08462338 `call empty; xor $0x1,%eax; test %al,%al; je` 直判。
                if (!result.m_dropItems.empty())
                {
                    Inven_Item& dst = m_rewards[memberIdx].m_items[i];
                    // [P167] ORIG `call begin(); call operator*`（迭代器槽
                    //   local_30/local_2c）⇒ 源写 `*v.begin()`，非 v.front()。
                    dst.setCopy(*result.m_dropItems.begin());

                    // ORIG 084623b5..0x485：operator* 结果 15×dword+1byte 块拷贝
                    //   到 -0xf5（Inven_Item 0x3d 字节平凡拷贝）⇒ 源是拷贝构造。
                    Inven_Item probe(*result.m_dropItems.begin());
                    // [P0] ORIG 无 itemList 具名局部（08462452 `mov 0xc(%eax),%eax`
                    //   直入 find_item，dm+0xc 直读）。
                    CItem* found =
                        G_CDataManager()->m_itemList->find_item(probe.m_addInfo);
                    if (found != 0)
                    {
                        // [P0] ORIG 无 grade/rarity 具名局部：08462477/0x484 两 call
                        //   结果经 %ebx/%edi 跨调用保活，按 R2L 序直传 UseLuckPoint 的
                        //   第 4/3 实参（084624a7 `mov %ebx,0xc(%esp)`）。
                        ::CLuckPoint::UseLuckPoint(party->get_user(memberIdx),
                                                 (unsigned char)probe.m_field1,
                                                 found->get_rarity(),
                                                 found->get_grade());
                    }
                }
            }
        }
        else
        {
            // ---- 随机掉落分支（ORIG +0x0f2：vtable+0x08 random）----
            // ref 上下文区（ORIG 0846205d..099：movb/movb/movb/movl/movw×4）
            ref.m_field35 = 0;
            ref.m_field34 = (char)param4;
            ref.m_rateSlot = 1;
            ref.m_rankBonusIndex = 1;                          // ORIG movl
            ref.m_goldBase = 1;                                // ORIG movw +0x3c
            ref.m_goldFactor = 1;                              // ORIG movw +0x3e
            *reinterpret_cast<unsigned short*>(&ref.m_dungeon) = 1;       // ORIG movw 低 16 位
            *(reinterpret_cast<unsigned short*>(&ref.m_dungeon) + 1) = 1; // ORIG movw 高 16 位

            stGenerateResult_t randomResult;                   // ORIG -0xb8 独立结果区
            // [P282 真 bug 修] ORIG 08462131 `mov (%eax),%edx; add $0xc,%edx;
            //   call *%ecx` ⇒ 本分支（roll<boundHigh、ref 0x10001 组）走
            //   **vtable+0x0c** = generateSpecificItem（_ZTV word[5]=0x0853a406）。
            // [P0] ORIG 无 genMgr/gen 具名局部（084620f0..0x12b 直链）。
            G_CDataManager()->getItemGenerator()
                ->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateSpecificItem(ref, randomResult);

            // [P171] ORIG 本分支无 empty 守卫（08462df..0x4fa 直接 begin+解引用）；
            //   empty 检查只在受控分支（ORIG 08462338）出现。
            if (party != 0)
            {
                // QuickParty 奖励登记（基础奖励类型 0xb：道具奖励槽）
                // [P167] ORIG 只调一次 begin（迭代器槽 local_38）却解引用两次
                //   ⇒ 源有具名迭代器复用；`(*it).m_...` 形（deEv，非 ptEv）。
                // [P171] ORIG 无 RewardData_Result::clear 调用。
                std::vector<Inven_Item>::iterator it =
                    randomResult.m_dropItems.begin();
                QuickParty::RewardData_Param rp(party);
                rp.m_param4 = dungeonMgr.getDungeon()->get_index();
                QuickParty::RewardData_Result rs;
                rs.m_value00 = (*it).m_addInfo2;
                party->set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rs);
                (*it).m_addInfo2 = rs.m_value00;
            }
            Inven_Item& dst = m_rewards[memberIdx].m_items[i];
            dst.setCopy(*randomResult.m_dropItems.begin());
            const int addInfo = dst.get_add_info();
            // ORIG：fildl addInfo/flds m_rewardGoldWeight/fmuls/fistpl（默认控制字就近舍入）
            dst.set_add_info((int)((float)addInfo *
                                   G_CDataManager()->m_deathTower.m_rewardGoldWeight));
        }
    }
