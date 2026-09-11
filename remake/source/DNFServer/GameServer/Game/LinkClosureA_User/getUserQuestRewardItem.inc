// ---------------------------------------------------------------------------
// CUser::getUserQuestRewardItem（ORIG T 0x0866e914 报告全文已读）
// 字段全部位于 Quest：m_fieldDC(+0xdc vector<int>)/m_fieldE8(+0xe8)/
// m_rewardItems(+0xec)/m_titleReward(+0xc4)；其余探查经由第二参 const CUser*
// 的 getCurCharacQuestR()->isClearQuest(idx)。
//   mode==1：命中任一已清除 idx 即返回 &m_rewardItems；
//   否则：全部 idx 已清除才返回 &m_rewardItems；
//   空 idx 表或不满足一律返回 &m_titleReward。
// ---------------------------------------------------------------------------
// b28d-66 [P281]：CUser.h:3055 已 static（Main 代落）⇒ 实参 0x8/0xc(%ebp) 与 ORIG 一致。
//   本轮修正：ORIG 每个分支各自 `const_iterator it = quest->m_fieldDC.begin()/end()`
//   （const vector ⇒ 一次 call 直构，槽 else=-0xc/-0x10、then=-0x14/-0x18），无 const_cast
//   非 const begin 引起的 iterator→const_iterator 转换构造；else 分支为
//   `for(; it!=end; ++it) if(!isClearQuest(*it)) break;`（ORIG xor $1 证 break 形）。
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
            // [P8S4 2026-09-11 负结论] ORIG 0x0866ea7f `jne 0x866ea2e`(回边) 之后紧跟
            // 0x0866ea81 `jmp 0x0866ea84`（跳过 0x0866ea83 nop 岛）再落函数尾
            // `mov 0x8(%ebp),%eax; add $0xc4,%eax`（0x0866ea84..8a）。4.4 把循环出口
            // 与函数尾合并为直落 ⇒ 恒缺 1 条。已试 P341 显式 `goto questRewardTail`
            // 尾标签（/tmp/r5/p8 复编）：4.4 仍折成直落，112=110 不变（无改善，
            // 已回滚）。判 N 类（4.1 nop 岛 + jmp 布局）封顶。
        }
    }
    return &const_cast<Quest*>(quest)->m_titleReward;  // [lea +0xc4]
}