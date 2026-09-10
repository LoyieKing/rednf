std::vector<RewardItemInfo>* CUser::getUserQuestRewardItem(
    const Quest* quest, const CUser* other)     // ORIG _ZN5CUser22…PKS_ 非 const
{
    // b28d-66 [P0]：ORIG 0x0866e914 全程直接重读形参 0x8/0xc(%ebp)（无槽往返）
    // ⇒ 原码无具名 `Quest* q` 局部，const_cast 仅用于返回值地址表达式。
    if (!quest->m_fieldDC.empty())                      // [0x866e91b xor $1]
    {
        // b28d-66：ORIG 布局 = `cmp $1; je <后置块>`（后置块槽 -0x14/-0x18）⇒ 原码
        // 条件取 `!= 1`（break-块 fall-through 槽 -0xc/-0x10），4.4 生同一极性。
        if (quest->m_fieldE8 != 1)                      // [cmp $1; je else(后置块)]
        {
            std::vector<int>::const_iterator it = quest->m_fieldDC.begin();  // [-0xc]
            std::vector<int>::const_iterator end = quest->m_fieldDC.end();   // [-0x10]
            for (; it != end; ++it)
            {
                if (!other->getCurCharacQuestR()->isClearQuest(*it))         // [xor $1]
                {
                    break;
                }
            }
            // b28d-66：ORIG 0x0866e9e3 为 `call eq; test; je +0x16f`（目标 = 空块
            // nop 中继岛，then 块尾另有 `jmp +0x170` 跳过该岛）——本形逐条匹配前 3 条，
            // 仅缺 4.1 的对齐 nop 岛+jmp（P223 试形产 `eq;xor;test;jne` 反劣，已回退）。
            if (it == end)                                 // [operator== P7]
            {
                return &const_cast<Quest*>(quest)->m_rewardItems;
            }
        }
        else
        {
            std::vector<int>::const_iterator it = quest->m_fieldDC.begin();  // [-0x14]
            std::vector<int>::const_iterator end = quest->m_fieldDC.end();   // [-0x18]
            for (; it != end; ++it)
            {
                if (other->getCurCharacQuestR()->isClearQuest(*it))
                {
                    return &const_cast<Quest*>(quest)->m_rewardItems;  // [lea +0xec]
                }
            }
        }
    }
    return &const_cast<Quest*>(quest)->m_titleReward;  // [lea +0xc4]
