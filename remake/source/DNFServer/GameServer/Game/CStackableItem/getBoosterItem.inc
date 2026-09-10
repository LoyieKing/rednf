void CStackableItem::getBoosterItem(
    std::vector<stStackableBoosterItemInfo_t>& out) const
{
    // ORIG 0x850f76e/0x850f793：外层 begin/end 在循环外各求值一次（P13）。
    std::vector<stStackableBoosterElement_t>::const_iterator it =
        m_booster.m_elements.begin();
    std::vector<stStackableBoosterElement_t>::const_iterator itEnd =
        m_booster.m_elements.end();
    for (; it != itEnd; ++it)
    {
        unsigned int i = 0;
        // ORIG 0x850f75e：m_count/m_prob 均按无符号比较（seta/setb）。
        // b28d-solo 实验：i 改 unsigned + 裸比较（ORIG seta 形）。
        while (it->m_count > i)
        {
            int rand = get_rand_int(it->m_prob);
            // ORIG 0x850f7e5/0x850f7f3：内层 begin/end 亦各求值一次（P13）。
            std::vector<stStackableBoosterItemInfo_t>::const_iterator it2 =
                it->m_items.begin();
            std::vector<stStackableBoosterItemInfo_t>::const_iterator it2End =
                it->m_items.end();
            for (; it2 != it2End; ++it2)
            {
                int idx = it2->m_itemIdx;
                if (static_cast<unsigned int>(rand)
                    < static_cast<unsigned int>(it2->m_prob))
                {
                    out.push_back(*it2);
                    break;
                }
            }
            ++i;
        }
    }
