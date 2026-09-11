// ----------------------------------------------------------------------------
// ORIG T 0x08688d26 _ZN5CUser16GetItemLimitInfoEi：
// begin/end 双迭代器（end 缓存）；元素 +0x15==itemIdx 返回元素指针；否则 0。
// ----------------------------------------------------------------------------
LimitItemUsage* CUser::GetItemLimitInfo(int itemIdx)
{
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin(),
         e = m_limitItemUsage.end(); it != e; it++)
    {
        if (it->m_field15 == itemIdx)
            return &*it;
    }
    return 0;
}