CAICharacter* CAICharacterList::getByAP(unsigned int ap)
{
    if (m_groupMap.empty())
        return 0;

    // [b28d-L14 实证] ORIG 0x0834a38e 全用 _Rb_tree_const_iterator（含 4 次
    // 转换构造 + const 版 operator--/at，P96/P270 反向：此处须 const_iterator）。
    APGroupMap::const_iterator it = m_groupMap.lower_bound((int)ap);
    // ORIG 0x0834ab66/0x0834aba9：end() 求值两次（比较 + 赋值后再自减）。
    if (it == m_groupMap.end())
    {
        it = m_groupMap.end();
        --it;
    }

    if (((unsigned int)it->first > ap) && it != m_groupMap.begin())
        --it;

    if (it->second.size() == 1)
        it->second.at(0);  // ORIG 汇编如此调用但未使用结果

    return it->second.at(get_rand_int((int)it->second.size()));
