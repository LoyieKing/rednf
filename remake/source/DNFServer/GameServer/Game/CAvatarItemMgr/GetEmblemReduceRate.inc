int CAvatarItemMgr::GetEmblemReduceRate(int key)   // ORIG 非 const
{
    // [b28d-38] P96/P270：const_iterator 引入转换构造 call（+10 条）⇒ 非 const iterator。
    // P222：ORIG 尾部 ret0 块（`mov $0x0,%eax; leave`）⇒ 正极性包裹（!=end fall-through）。
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    if (it != m_emblemEnduranceMap.end())
        return it->second;
    return 0;
}