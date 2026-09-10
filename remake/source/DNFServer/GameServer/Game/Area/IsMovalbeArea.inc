bool Area::IsMovalbeArea(int vill, int area) const
{
    std::vector<std::pair<int, int> >::const_iterator it = m_moveAbleArea.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = m_moveAbleArea.end();
    for (; it != itEnd; ++it)
    {
        const std::pair<int, int>& p = *it;   // ORIG 解引用一次（绑定引用）
        if (p.first == vill && p.second == area)
            return true;
    }
    return false;
