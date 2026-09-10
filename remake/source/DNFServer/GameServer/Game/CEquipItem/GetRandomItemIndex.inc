int RandomItemTable::GetRandomItemIndex(int index) const
{
    // [b28c-21 代落 C28L35 spec] ORIG 增量用后置 ++（operator++(int) 三参形）。
    for (std::vector<std::pair<int, int> >::const_iterator it = m_vec4.begin();
         it != m_vec4.end(); it++)
    {
        if (index < it->second)
            return it->first;
    }
    return 0;
