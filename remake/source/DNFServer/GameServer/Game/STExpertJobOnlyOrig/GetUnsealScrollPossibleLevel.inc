// 同构四联（find 命中返 second、未中返 -1；ORIG 为逐调用完整 find/end 展开，
// 不抽 helper 以保持函数体形态）：
int STExpertJobScript::GetUnsealScrollPossibleLevel(int level)
{
    // ORIG：非 const iterator（_Rb_tree_iterator 直用，无 const 转换）。
    std::map<int, unsigned short>::iterator it = m_unsealScrollMap.find(level);
    if (it == m_unsealScrollMap.end())
        return -1;
    return static_cast<int>(it->second);
}