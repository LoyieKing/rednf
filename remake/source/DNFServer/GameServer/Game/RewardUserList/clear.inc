void CRewardUserList::clear()
{
    m_map.clear();     // ORIG 先 map（0x081bc04e）
    m_index.clear();   // 后 deque（0x081bc05c）
}