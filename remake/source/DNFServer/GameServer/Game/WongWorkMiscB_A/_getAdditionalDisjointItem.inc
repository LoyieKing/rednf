void CItemUpgrade::_getAdditionalDisjointItem(
    unsigned int up, const CItem* itemScript,
    std::vector<std::pair<int, int> >& items,
    const upgrade_table_t& table, bool flag)
{
    (void)itemScript;
    try
    {
        STResultItemCnt elem = table.m_destruction.m_vec.at(up);   // at 越界抛异常 → catch 吞掉并 trace
        unsigned long slotIdx;
        unsigned int count;
        if ((int)(short)elem.m_a != up)
            goto L_island1;
        slotIdx = table.m_destruction.m_field58;
        count = 0;
        if (flag)
            count = (unsigned int)(short)elem.m_c;
        else
            count = (unsigned int)(short)elem.m_b;
        if (count == 0)
            goto L_island2;
        items.push_back(std::make_pair(slotIdx, count));
    L_island1:;
    L_island2:;
    }
    catch (...)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x121, 5)("3RD_CHRONICLE : OUT OF RANGE!!");
    }
