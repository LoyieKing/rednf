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
            int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x18]
            int penalty = 100;                             // [-0x14]
            int gold = 0;                                  // [-0x10]
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
    } while (!(it == end));                                // [尾判回环，ORIG operator==]

    return 0;
