bool Event_Upgrade_Jar::CheckMaxLuckyLevel(Inven_Item& item, unsigned long upgrade)
{
    // [b28c-35] P0 清绑：删 3 枚 `register bool asm("al")` 显式绑定（本波定稿：
    // 任何情形不得保留显式绑定，宁 AE→DIFF 也要清）。改自然嵌套 if（P225 拆嵌套
    // if 复现逐条件 setcc 物化）。
    if (m_materialIndices.size() > 1)
    {
        if (m_materialIndices[1] == upgrade)
        {
            if ((unsigned int)item.GetUpgrade() >= (unsigned int)m_upgradeMax)
            {
                return false;
            }
        }
    }
    return true;
