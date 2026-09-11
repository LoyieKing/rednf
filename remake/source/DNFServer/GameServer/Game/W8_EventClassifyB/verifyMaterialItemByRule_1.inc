// verifyMaterialItemByRule（ORIG 0x0810eb30）
// flag08==0：内层命中即收集并 break；外层在 materials 非空时终止整个双重循环。
// flag08!=0：全量收集；收集数!=规则数 → 0x16。不缓存 script/rule（P36/P48：
// ORIG 对 pairs0c[r] 三次 operator[]）。命中条件两元素装载：first 先、second 后。
// [b28d-solo-W8_EventClassifyB 判词·归零] 修法=块序反写：ORIG 0x0810eb46
// `test %al,%al; je +0x190` ⇒ flag08!=0 分支直落在前、flag08==0（含 break）
// 分支后置；条件反写 + 两分支体互换后归一化 ndiff=1（仅尾部对齐 nop，§6.1 豁免）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR → n1_v 新口径 AE（gap=+1，blocks=0）⇒ 旧判词作废。
int CExchangeRandomItemReward::verifyMaterialItemByRule(
    CUser* user, std::vector<int>& slots,
    std::vector<std::pair<int, int> >& materials)
{
    // ORIG 0x0810eb46 `test %al,%al; je +0x190`：flag08!=0 分支直落在前、
    // flag08==0 分支（含 break）位于 +0x190 ⇒ 条件反写 + 分支体互换。
    if (static_cast<ExchangeRandomItemRewardScript*>(m_script1c)->m_flag08 != 0)
    {
        size_t r = 0;
        while (r < static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                       ->m_pairs0c.size())
        {
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(1, slots[s]);
                if ((unsigned int)static_cast<ExchangeRandomItemRewardScript*>(
                        m_script1c)->m_pairs0c[r].first ==
                        (unsigned int)slotItem.m_addInfo &&
                    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second <= slotItem.m_addInfo2)
                {
                    materials.push_back(std::make_pair(
                        slots[s],
                        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second));
                }
                ++s;
            }
            ++r;
        }
        if (materials.size() !=
            static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                ->m_pairs0c.size())
            return 0x16;
    }
    else
    {
        size_t r = 0;
        while (r < static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                       ->m_pairs0c.size())
        {
            size_t s = 0;
            while (s < slots.size())
            {
                Inven_Item slotItem =
                    user->getCurCharacInvenR()->GetInvenSlot(1, slots[s]);
                if ((unsigned int)static_cast<ExchangeRandomItemRewardScript*>(
                        m_script1c)->m_pairs0c[r].first ==
                        (unsigned int)slotItem.m_addInfo &&
                    static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second <= slotItem.m_addInfo2)
                {
                    materials.push_back(std::make_pair(
                        slots[s],
                        static_cast<ExchangeRandomItemRewardScript*>(m_script1c)
                            ->m_pairs0c[r].second));
                    break;
                }
                ++s;
            }
            if (!materials.empty())
                break;
            ++r;
        }
    }
    if (materials.empty())
        return 0x16;
    return 0;
}