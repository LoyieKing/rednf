// ==============================================================================
// CPlayData::generateRewardItem（ORIG T 0x08461fc0）
//
// 期望掉落条数写入 m_rewards[memberIdx].m_itemCnt；循环 count 次投掷
// get_rand_int(1000)：rnd>rate 高阶掉落分支的概率锚点为 DataManager+0x5e20 /
// +0x5e24（乘 param5 比较）= m_deathTower.m_rewardItemProb[0]/[1]（0x5198+0xc88
// 起 int 数组，ORIG `mov 0x5e20(%eax),%eax` int 宽实证）；计数率因子
// +0x5e2c = m_deathTower.m_rewardGoldWeight（ORIG `flds` float）。**受控分支**
// （roll>=prob0*param5）经生成器虚槽 +0x08 = generateRandomItem、**随机分支**经
// 虚槽 +0x0c = generateSpecificItem（P282 实证，见函数头 2026-09-10 判词；旧注释
// 把两者写反）。受控分支额外触发幸运点
// 扣减（::CLuckPoint::UseLuckPoint）。ref 对象由其 ctor 初始化，上下文区按
// 具名字段写入（宽度逐条对齐 ORIG：movb/movw/movl，见
// WongWork__CDeathTower__CPlayData/generateRewardItem.md 0846205d..099 /
// 084622bc..328）。
// ==============================================================================
// [b28d-24 历史] DIFF 残380: P167 迭代器写法差（MISSING begin/deEv×5；EXTRA front/C1/D1/G_CDataManager 等）。
// [2026-09-10 round5 定稿] 真 bug 修 + P167 修（P282 流程）：
//   ① **两分支虚调目标互换（真 bug）**：P282 五步核 _ZTVN8WongWork14CItemGeneratorE
//      @0x08c9a788 —— word[2]=0x0853a3f0 setDropRatio / word[3]=0x0853a3f6 loadScript /
//      word[4]=0x0853a400 generateRandomItem / word[5]=0x0853a406 generateSpecificItem。
//      ORIG 08462311 段 `mov (%eax),%edx; add $0x8,%edx; call *%ecx` ⇒ **受控分支
//      （ref 0x3f800000、roll>=prob0*param5）调 generateRandomItem**；ORIG 08462131 段
//      `add $0xc,%edx` ⇒ 随机分支（ref 0x10001 组）调 generateSpecificItem —— 旧重建
//      恰好相反（两分支掉落语义颠倒）。
//   ② 条件符号性：ORIG `cmp -0x20(%ebp),%eax; seta %al; test %al,%al; je` 为 **unsigned**
//      （prob0*param5 <= roll ⇒ 受控）；旧重建写 `(int)boundHigh` signed 比较 + 二段
//      `||` + boundHigh 具名槽。prob1 内层 ORIG 只求值一次 G_CDataManager（旧重建
//      `<= && - != 0` 两次 ⇒ EXTRA +1）。
//   ③ P167：`.front()`→`*v.begin()`（ORIG begin×4/deEv×5）；`probe` 由 setCopy 改
//      **拷贝构造**（ORIG 084623b5..0x43f = 15×dword+1byte 块拷贝到 -0xf5）；随机分支
//      具名迭代器复用（ORIG 只 begin 一次却 deEv 两次）；随机分支 ORIG 无 `empty()`
//      守卫、无 `RewardData_Result::clear`。
//   ⇒ p167 差集 16→12 项（begin/deEv/front/empty/setCopy/G_CDataManager 全清）、
//      missing_in_game 25→24。
//   ④ [配方 C05② 正向（A27 机理）+ P0] 删 6 处 ORIG 无的具名局部（dropEmpty 直判
//      `!empty()`、itemList/grade/rarity 内联、genMgr→gen 链内联）⇒ our **388→357**
//      （ORIG 347，gap −41→**−10**）。
//   残余 = C1/C2·D1/D2 变体（P93 同址伪差：0814ab06 C2==C1、0814ab4c D2==D1）
//      + setcc 物化（ORIG seta×2/test×2，4.4 恒直跳，P197④ 族，等价 −4 条）+ ref 末
//      两 short 经 lea+add（stGenerateRefData_t 字段建模 +4）+ 杂项 ⇒ N 封顶。
//   ⑤ [C05 边界留痕] 本件 grade/rarity 在 ORIG 为**寄存器保活**（0846247c `mov %eax,%ebx`
//      / 08462489 `mov %eax,%edi`，经 `084624a7 mov %ebx,0xc(%esp)` 直传 varargs）⇒ 与 C07
//      （ORIG 用槽则写具名 int 局部）方向**相反**：此处必须内联（已按此改）。C05③ 闸门
//      （岛差 ≥2）与 C05① 多层尾块均不适用（各早退为共享 epilogue，ORIG 同形）。
// [M1S3EmblemLink 2026-09-11 试后回滚] 删独立 `::CLuckPoint luck;`（改传
//   `&result.m_luckPoint`，照 ORIG 0x08462019 `lea -0x88(%ebp),%eax; add $0xc,%eax`）
//   ⇒ 本函数 357→343（ORIG 347）、p167 47/47 归零，**但 TU 四数劣化**
//   （identical 16→7、ae 47→29、missing 24→12：该 odr-use 是 ~CLuckPoint 与
//   vector<CUser*>/map<int,…> 共 27 个 COMDAT 符号的唯一发射源）⇒ 按闸门回滚，
//   负结论见函数体 `::CLuckPoint luck;` 处注释。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b）→ 试=P342 四步复核（判据终版）：
//   ①分支号：本 TU 无 `_Destroy_aux<ILb0>` 反分支 ②ORIG 全 ELF：`CLuckPoint::CLuckPoint()`
//   @0x08550884 **T**、`~CLuckPoint()` @0x085508ec **T** + `global constructors keyed to
//   CLuckPoint::CLuckPoint()` @0x08550fad ⇒ ORIG 侧确有静态 odr-use 链 ③我方全库 1431 个
//   .o 中**仅本 TU** 发射 `_ZN10CLuckPointD1Ev`(W)/D2(W) ⇒ 删即丢（与 M1S3 实测
//   identical 16→7 / ae 47→29 一致）④p167 该 3 项 EXTRA 的代价 < 丢失 15 个匹配符号
//   ⇒ 维持 N18b，`::CLuckPoint luck;` 必须保留（除非在别的 TU 重建 odr-use 链）。
//   ORIG 0x08461fc0
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

        // [M1S3EmblemLink 2026-09-11 负结论 · p167 与 TU 四数冲突实测]
        //   ORIG 0x08462019 `lea -0x88(%ebp),%eax; add $0xc,%eax; mov %eax,
        //   0x4(%esp); … call CParty::GetLuckPoint(CLuckPoint*, bool)` 传的是
        //   result 成员 m_luckPoint(+0x0c)，且 ORIG 无独立 CLuckPoint 局部；
        //   改 `&result.m_luckPoint` 后本函数 357→343、p167 47/47 归零（真对齐），
        //   **但 TU 四数劣化**：identical 16→7 / ae 47→29 / missing 24→12 ——
        //   因 `::CLuckPoint luck;` 是本 TU 内联销毁 CLuckPoint 的唯一 odr-use，
        //   它消失后 ~CLuckPoint 及 std::vector<CUser*>、std::map<int,…> 的
        //   27 个 COMDAT 符号不再发射（而 ORIG 本 TU 含这些符号）。
        //   按「四数不得劣化」闸门回滚；保留 `::CLuckPoint luck;` 直证形。
        ::CLuckPoint luck;   // sizeof 0x24；ORIG 隐式 ~CLuckPoint COMDAT 依赖此 odr-use
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
        // [P6AuditOldN-S1 2026-09-11 负结论 P196 反形] 试 `!(X > roll)`（语义等价的
        //   严格反形，目标 ORIG 0x0846204f `cmp -0x20(%ebp),%eax; seta %al`）⇒ 实测
        //   our 357→**358**（gap −10→−11，反劣 +1）⇒ 已回滚，维持 N18b/P197④ 封顶。
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
                // [G23-P355 落地] 原为 `*reinterpret_cast<int*>(&ref.m_goldBase) = 0;`
                // —— 退化为指针写 ⇒ 4.4 恒产 `lea -0x140(%ebp),%eax; add $0x3c,%eax;
                // movl $0x0,(%eax)`（3 条）；改 union 的**整字具名视图** `m_field3c`
                // 直赋 ⇒ 复现 ORIG 单条 disp 直清 `movl $0x0,-0x104(%ebp)`（P355）。
                ref.m_field3c = 0;                             // ORIG movl $0 覆盖 +0x3c/+0x3e

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
}