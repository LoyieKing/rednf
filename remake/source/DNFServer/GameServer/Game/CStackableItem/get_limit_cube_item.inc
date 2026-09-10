void CStackableItem::get_limit_cube_item(std::pair<int, int>& out)
{
    unsigned long range;
    int itemIdx = 0;
    int count = 0;
    int total = 0;
    std::vector<stLimitCubeResult>::const_iterator it =
        m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        total += r.m_prob;
        it++;
    }
    range = total;
    unsigned int rand = m_pRand->randInt(range);
    // b28d-solo 修法：sum unsigned（裸比较翻操作数序，P218/P310 扩展；见函数头判词）。
    unsigned int sum = 0;
    it = m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        const stLimitCubeResult& r = *it;
        sum += r.m_prob;
        if (rand <= sum)
        {
            itemIdx = r.m_itemIdx;
            count = r.m_count;
            break;
        }
        it++;
    }
    out.first = itemIdx;
    out.second = count;
