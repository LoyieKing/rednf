// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_gold（报告全文已读）
// m_b8 gate: quest+0xb8==0 或 ==0x19 才发放；遍历 m_titleReward(+0xc4) 首个
// m_itemIdx==0 元素计算金币；quest+0x104=m_field104 为金币倍率。
// [2026-09-11 本轮] P331 声明序修正（levelDelta/penalty/gold 裸声明提前到 itemIdx
//   之前，itemIdx 由最深 -0x18 归位到 ORIG 最浅 -0xc）：ndiff 17→**6**、gap −1→0、
//   insns 133→132=ORIG、槽全等。残余（均 N，语义等价）：①循环回边 ORIG
//   `call operator==; test; je loop` vs 4.4 `xor $1; test; jne`（N122/N292）；
//   ②ORIG 循环头前 `jmp <T>`+nop 与尾块 `mov $0; jmp` 的 4.1 布局（P223 族）。
//   试 `it != end`：ndiff 6→5、gap→0，但 p167 出现 MISSING(operator== 0x0869b233)/
//   EXTRA(operator!=) —— ORIG 无 ne 实例 ⇒ 按 P167 符号集口径回滚。
// ---------------------------------------------------------------------------
int CUser::compute_quest_basic_reward_gold(Quest* quest)
{
    if (!(quest->m_fieldB8 == 0 || quest->m_fieldB8 == 0x19))
    {
        return 0;                                          // [0x866e608]
    }

    // b28d-65 [N86]：ORIG 0x0866e61b 段为 begin()→临时 iterator 槽 + 转换构造两段
    // （4.1 不做 copy-elision）；4.4 直构进目标槽 ⇒ 少 2 条 + 迭代器槽号连锁。
    // 实测三形（=初始化 / ()初始化 / 先声明后赋值）均不可翻（后两形更劣 142 insn）。
    std::vector<RewardItemInfo>::const_iterator it = quest->m_titleReward.begin();
    std::vector<RewardItemInfo>::const_iterator end = quest->m_titleReward.end();

    do                                                     // [do-while P25]
    {
        if (it == end)
        {
            return 0;                                      // [P16 早退]
        }
        // b28d-66 [P0]：ORIG `mov (%eax),%eax; mov %eax,-0xc; cmpl $0,-0xc; je`
        // ⇒ 原码有具名局部（itemIdx 占 -0xc 槽），非内联 test/sete 形。
        // [P331 2026-09-11] ORIG 槽序：levelDelta(-0x18 最深) / penalty(-0x14) /
        //   gold(-0x10) / itemIdx(-0xc 最浅)。4.4 同尺寸标量区按「先声明 → 更深」排
        //   ⇒ 把 levelDelta/penalty/gold 改成**裸声明**提前（未初始化声明零指令、
        //   仅占槽，P328⑤），itemIdx 由最深 -0x18 回到最浅 -0xc（原差 0xc 的整段平移）。
        int levelDelta;
        int penalty;
        int gold;
        int itemIdx = it->m_itemIdx;                       // [-0xc]
        // b28d-66 真语义修正 + 布局：ORIG `cmpl; je +0xea`（命中块为跳转目标，
        // 以 return 结束），未命中遍历到末尾走回边后的 `return 0`；旧代码把计算
        // 放循环外，导致「遍历完未找到」也返回金币值。
        if (itemIdx != 0)
        {
            ++it;
        }
        else
        {
            levelDelta = get_charac_level() - quest->m_levelMin; // [-0x18]
            penalty = 100;                                 // [-0x14]
            gold = 0;                                      // [-0x10]
            // ORIG `movzbl 0x130; xor $1; test; je <后置块>` ⇒ 原码条件为
            // `if (!m_field130)`（field==1 跳后置 getRewardGold(get_charac_level())）。
            if (!quest->m_field130)                        // [xor $1 极性]
            {
                penalty = compute_level_penalty(levelDelta);   // [0x866e713]
                gold = G_CDataManager()->m_questParameter.getRewardGold(quest->m_levelMin);
            }
            else
            {
                gold = G_CDataManager()->m_questParameter.getRewardGold(get_charac_level());
            }

            return gold * penalty / 100 * quest->m_field104 / 100; // [两段 imul/div]
        }
    // [2026-09-11 试形·回滚] ORIG 回边 = `call operator==; test; je loop`（4.1 对 !(a==b)
    //   直接反转跳转无 xor）；4.4 对同源码恒发 `xor $1; test; jne`（N122/N292）。
    //   试 `it != end`：ndiff 6→5、gap −1→0，但 p167 由「C1/C2 同址别名」变为
    //   MISSING(eq 0x0869b233) + EXTRA(ne 新符号) —— ORIG 无 ne 实例，违反 P167
    //   符号集口径 ⇒ 回滚，维持 `!(it == end)`（xor 属 N 类封顶）。
    } while (!(it == end));                                // [尾判回环，ORIG operator==]

    return 0;
}