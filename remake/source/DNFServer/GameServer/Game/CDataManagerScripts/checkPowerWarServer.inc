// ============================================================================
// checkPowerWarServer（ORIG T 0x08a59db4）：遍历 m_powerWarServerList
// （+0x9c，vector<pair<int,int>>），pair.first==a && pair.second==b → 1，
// 否则 0。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①`std::pair<int,int> p;` 默认构造于 begin 前（P50 复活；ORIG 0x81b5c8e=C1Ev）；②end 缓存（P13）；③`p = *it` 结构赋值替代两次 ptEv；ORIG 0x08a59db4。
bool PowerParameterScript::checkPowerWarServer(int a, int b)
{
    std::pair<int, int> p;
    std::vector<std::pair<int, int> >::iterator it = m_powerWarServerList.begin();
    std::vector<std::pair<int, int> >::iterator e = m_powerWarServerList.end();
    for (; it != e; ++it)
    {
        p = *it;
        if (p.first == a && p.second == b)
            return true;
    }
    return false;
}