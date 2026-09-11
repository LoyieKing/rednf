// ---- selectReward（ORIG 0x0831c872）----------------------------------------
// key=m_keys[keyIndex] 查 m_rewardMap；keyIndex∈{0,2,4}（startBreakAway 三分支）。
// point<0 返 0；遍历 list：
//   rewards ← {item.m_field8, m_fieldc, m_field10, point}（每轮重写暂存）
//   if (point < item.m_field0) return false;
//   if (item.m_field0 <= point <= item.m_field4) set + return true;
// 循环耗尽：仍按最后一轮暂存值调用 set 并返回 1（ORIG asm fallthrough 行为）。
bool CBreakAwayPreventSystem::selectReward(int keyIndex, int point, CUser* user)
{
    // ORIG 0x0831c872：map/_List 迭代器均为非 const（_Rb_tree_iterator/_List_iterator）。
    std::map<STRewardDataKey, STRewardDataList>::iterator it =
        m_rewardMap.find(m_keys[keyIndex]);
    if (it == m_rewardMap.end())
        return false;
    // ORIG -0x18 具名 list 指针（0x0831c8f1 operator-> + add $4 落槽，P0 有槽）。
    std::list<STRewardData>* pList = &it->second.m_list;
    // ORIG -0x14 具名 point 副本（0x0831c8f7 mov 0x10(%ebp) → 槽，后续反复重读）。
    int rewardPoint = point;
    if (rewardPoint < 0)
        return false;

    STBreakAwayRewards rewards;   // ORIG STBreakAwayRewards ctor 清零
    // ORIG -0x28 死局部：keyIndex 写后未读（0x0831c918 mov 0xc(%ebp) → 槽）。
    // [M3S5 2026-09-11 · P339 提头试形⇒否] 把 keyIndexKeep 提为**函数头第一个声明**
    //   （期望按「头内先声明更深」落到 ORIG 的 -0x28）⇒ 实测 selectReward
    //   NEAR(143)→**DIFF**（TU 四数 near 1→0、diff 2→3）⇒ 已回滚。残余：ORIG 槽序
    //   li(-0x3c)/rewards(-0x38)/keyIndex(-0x28)/it(-0x24)….pList(-0x18)/point(-0x14)，
    //   我方对应各深 4~7（keyIndex 反向浅 0x14 但提头无效）⇒ 判 N82/N95 槽阶梯封顶。
    int keyIndexKeep = keyIndex;
    (void)keyIndexKeep;
    for (std::list<STRewardData>::iterator li = pList->begin();
         li != pList->end(); ++li)
    {
        // ORIG 0x0831c938：每轮 operator-> 逐字段读 5 次（P157，无 entry 引用缓存）；
        // 读序 field0→field4→field8→fieldc→field10→point。
        const int minVal = li->m_field0;
        const int maxVal = li->m_field4;
        rewards.m_rewardItemId   = li->m_field8;
        rewards.m_rewardAItemCnt = li->m_fieldc;
        rewards.m_luckPoint      = li->m_field10;
        rewards.m_fieldc         = rewardPoint;
        // ORIG 0x0831c992：先 `cmp minVal,p; jle 继续` + 失败直落 return 0（早退形）。
        if (minVal > rewardPoint)
            return false;
        if (minVal <= rewardPoint && maxVal >= rewardPoint)
        {
            rewards.m_rewardItemId   = li->m_field8;
            rewards.m_rewardAItemCnt = li->m_fieldc;
            rewards.m_luckPoint      = li->m_field10;
            rewards.m_fieldc         = rewardPoint;
            user->setBreakAwayRewards(rewards);
            return true;
        }
    }
    user->setBreakAwayRewards(rewards);
    return true;
}