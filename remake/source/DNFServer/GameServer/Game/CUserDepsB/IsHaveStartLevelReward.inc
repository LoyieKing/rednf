// ORIG T 0x08687d18 sz 0x54：遍历 m_table 指针数组（0x46 项），
// 元素为 0 → 空槽终止返回 false；元素 ==1 → 有起始奖励。
bool CLevelRewardTableMgr::IsHaveStartLevelReward()
{
    int i = 0;
    while (i <= 0x45)
    {
        if (m_table->m_levelKeys[i] == 0)
            return false;
        if (m_table->m_levelKeys[i] == 1)
            return true;
        ++i;
    }
    return false;
}