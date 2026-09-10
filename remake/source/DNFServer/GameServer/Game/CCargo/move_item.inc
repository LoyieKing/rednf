bool CCargo::move_item(int from, int to)
{
    if (!(IsOperatorable(from) && IsOperatorable(to)))
        return 0;
    if (m_pItems[from].m_addInfo == 0 && m_pItems[to].m_addInfo == 0)
        return 0;
    Inven_Item tmp;
    tmp = m_pItems[from];
    m_pItems[from] = m_pItems[to];
    m_pItems[to] = tmp;
    m_sorted = 0;
    return 1;
