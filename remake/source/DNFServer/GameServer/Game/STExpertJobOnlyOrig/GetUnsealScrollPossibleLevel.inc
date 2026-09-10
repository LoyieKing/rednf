int STExpertJobScript::GetUnsealScrollPossibleLevel(int level)
{
    // ORIG：非 const iterator（_Rb_tree_iterator 直用，无 const 转换）。
    std::map<int, unsigned short>::iterator it = m_unsealScrollMap.find(level);
    if (it == m_unsealScrollMap.end())
        return -1;
    return static_cast<int>(it->second);
