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
