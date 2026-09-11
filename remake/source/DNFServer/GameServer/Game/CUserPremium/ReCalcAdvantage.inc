// [b28d-solo-CUserPremium] 修法（P167/P0/P211/P214①/P254）：
// ①插入路径改 make_pair<int&,int&> + pair<const int,int> 右值转换构造、
//   else 用 `it->second +=`（ORIG 单次 ptEv）⇒ 调用点差集清零（23/23 CLEAR）；
// ②overEquip body 的 lhs 索引 + max 第一实参各重算 vec[]（ORIG 3 调用点）；
// ③去 int now 中转（nowT 单槽）；④+0x85e/+0x850 由 Main 代落 CUser.h 两字段
//   short→unsigned short（ORIG 0x0812cdb4/0x0860939e 两处 movzwl 证据；探针
//   /tmp/p_u16b.cpp f6 vs f7/f8：目标 short ⇒ `movzwl→%eax; mov→%edx` 两段，
//   目标 unsigned short ⇒ `movzwl→%edx` 单段）；⑤m_slots enum 化复现两段
//   中转（P254）。
// 终判（b28d 复测）：IDENTICAL 482/482（ndiff 0，gap 0，槽位集全等），
// P167 CLEAR（23/23）——旧判词「余 3 处 N18a/N-86b 封顶」已由④推翻。
void CUserPremium::ReCalcAdvantage()
{
    // ORIG 0x086ae10e（O=487）：形态按 T-C9-1 全去缓存——g_SPremiumInfo[type]
    // 链（type*0x120=(t*9)<<5）与 m_entries[slot]（slot*0x14）在每个字段读取点
    // 独立重算（ORIG +0x142/0x1bc/0x1ee/0x237/0x283...每处 11-14 条），不使用
    // def/groupDef/entries 指针局部。清零段：0x850(movw)/0x854/0x858/0x85c/
    // 0x8bc/memset(0x864,0x58)/clear×2/map.clear/0x85e/0x860。
    m_outFatigueRate = 0;
    m_outPremiumCount = 0;
    m_outExpRate = 0;
    m_overEquipFlag = false;
    m_field8bc = 0;
    memset(m_overEquipableLevel, 0, sizeof(m_overEquipableLevel));
    m_additionalBase.clear();
    m_additionalInfo.clear();
    m_returnItemMap.clear();
    m_overSkillLevel = 0;
    m_ceraDiscountRate = 0;

    // [b28d-solo-CUserPremium] P0：ORIG 只有 nowT 一槽（`mov %eax,-0x3c(%ebp)`
    // 后直接 lea 传址）；具名 int now 中转多 1 槽 + 1 条 mov（ndiff -3）。
    time_t nowT = (time_t)GlobalData::s_systemTime_.getCurSec();
    tm localTm;
    localtime_r(&nowT, &localTm);

    for (int slot = 1; slot <= 0x69; ++slot)
    {
        if (m_entries[slot].m_type == 0)
            continue;
        if (g_SPremiumInfo[m_entries[slot].m_type].m_kind != 1)
            continue;
        if (m_entries[slot].m_state == 2)
            continue;
        if (!_CheckApply(m_entries[slot].m_type, localTm))
            continue;

        if (g_SPremiumInfo[m_entries[slot].m_type].m_needDailyCheck != 0)
            m_overEquipFlag = true;

        // +0x85e/+0x860：unsigned short max，ORIG 调用站点 max<unsigned short>
        // [b28d-solo-CUserPremium] P0：ORIG 对 +0x85e 直接取址（`add $0x85e,%eax`）
        // 传引用；`static_cast<unsigned short>(字段)` 产生临时槽（movzwl+movw+lea）。
        m_overSkillLevel =
            std::max(reinterpret_cast<unsigned short&>(m_overSkillLevel),
                     g_SPremiumInfo[m_entries[slot].m_type].m_overSkillLevel);
        m_ceraDiscountRate = std::max(
            m_ceraDiscountRate,
            g_SPremiumInfo[m_entries[slot].m_type].m_field5a);

        m_outFatigueRate =
            (unsigned short)m_outFatigueRate +
            g_SPremiumInfo[m_entries[slot].m_type].m_fatigueRate;
        m_outExpRate =
            m_outExpRate + g_SPremiumInfo[m_entries[slot].m_type].m_expRate;
        m_outPremiumCount = m_outPremiumCount +
            g_SPremiumInfo[m_entries[slot].m_type].m_premiumCount;
        if (g_SPremiumInfo[m_entries[slot].m_type].m_overEquipFlag != 0)
            m_field8bc = 1;

        // ORIG 组循以 slot（非 type）索引 g_SPremiumInfo（+0x120*(slot) 链
        // 每次重算，+0x378/+0x42a/+0x4a3/+0x5d1...）；pair 元素 (t*0x24+i+0xc)
        // *8 链独立。
        for (int i = 0;
             i < g_SPremiumInfo[slot].m_field64; ++i)
        {
            int j = 0;
            while (j < (int)g_vecPremiumOverEquipableInfo
                          .operator[](
                              g_SPremiumInfo[slot]
                                  .m_returnPairs68[i]
                                  .first)
                          .m_count)
            {
                // [b28d-solo-CUserPremium] P167：ORIG 该 body 有**两个**
                // vector<SPremiumOverEquipableInfo>::operator[] 调用点
                // （lhs 索引 + max 第一实参各重算一次，无 category 具名局部；
                // ORIG 共 3 处调用 vs 旧形 2 处 ⇒ MISSING×1）。
                m_overEquipableLevel[
                    g_vecPremiumOverEquipableInfo
                        .operator[](
                            g_SPremiumInfo[slot].m_returnPairs68[i].first)
                        .m_slots[j]] =
                    std::max(
                        m_overEquipableLevel[
                            g_vecPremiumOverEquipableInfo
                                .operator[](
                                    g_SPremiumInfo[slot].m_returnPairs68[i].first)
                                .m_slots[j]],
                        g_SPremiumInfo[slot].m_returnPairs68[i].second);
                j++;
            }
        }

        for (int i = 0;
             i < g_SPremiumInfo[slot].m_field90; ++i)
        {
            int j = 0;
            while (j < (int)g_vecPremiumReturnItemInfo
                          .operator[](
                              g_SPremiumInfo[slot]
                                  .m_returnPairs94[i]
                                  .first)
                          .m_count)
            {
                int itemIndex =
                    g_vecPremiumReturnItemInfo
                        .operator[](
                            g_SPremiumInfo[slot].m_returnPairs94[i].first)
                        .m_items[j];
                std::map<int, int>::iterator it =
                    m_returnItemMap.find(itemIndex);
                if (it == m_returnItemMap.end())
                {
                    // [b28d-solo-CUserPremium] P167：ORIG 调 make_pair<int&,int&>
                    // + pair<const int,int>::C1<int,int>(pair<int,int>&&)（右值
                    // 转换构造）；旧具名 pair 左值走拷贝版（EXTRA 两符号）。
                    m_returnItemMap.insert(std::make_pair(itemIndex,
                        g_SPremiumInfo[slot].m_returnPairs94[i].second));
                }
                else
                {
                    // [b28d-solo-CUserPremium] P167：ORIG 只发 1 次 ptEv
                    // （`it->second += x` 复合赋值形）；写成 `it->second =
                    // it->second + x` 4.4 会发 2 次 ptEv。
                    it->second +=
                        g_SPremiumInfo[slot].m_returnPairs94[i].second;
                }
                j++;
            }
        }

        m_additionalBase +=
            g_SPremiumInfo[m_entries[slot].m_type].m_additionalInfo;
    }
}