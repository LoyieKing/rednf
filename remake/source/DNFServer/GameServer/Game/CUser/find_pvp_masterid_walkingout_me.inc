bool CUser::find_pvp_masterid_walkingout_me(unsigned int id)
{
    if (m_set8cfe4.empty())
        return false;
    // ORIG 0x865cf32 `test;je → mov $1 尾岛`：eq + 命中返常量 1（技巧 17 fall 块在前 +
    // 技巧 30 尾岛）；`return *it` 反产 pt/deref 块。
    std::set<unsigned int>::iterator it = m_set8cfe4.find(id);
    if (it == m_set8cfe4.end())
        return false;
    return true;
}