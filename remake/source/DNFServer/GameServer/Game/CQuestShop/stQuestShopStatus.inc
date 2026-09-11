// ORIG C1 0x891b948（objdump 直转）：4×string + vector 构造后 clear()。
stQuestShopStatus::stQuestShopStatus()
    : m_name(), m_name2(), m_explain(), m_icon(), m_levelInfos()
{
    clear();
}