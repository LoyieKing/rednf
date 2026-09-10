void CItemDictionary::setGainItemFlag(CUser* user, unsigned int item)
{
    if (user == 0)
        return;
    CItem* ci = G_CDataManager()->find_item(static_cast<int>(item));
    // 语义纠错（b28 lane9）：ORIG 0x0811d833 处 `cmpl $0,-0x10; je <mov $1>`——
    // find_item 返回空指针时物化 bVar1=true 直接早退（与 is_stackable 命中同出口）；
    // 旧写法 `ci != 0 && is_stackable()` 短路使 ci==0 落入主体、错调 setItemCheckFlag。
    // 恢复 Ghidra C 原式 (ci == 0 || is_stackable) 为真即返回。
    if (ci == 0 || ci->is_stackable())
    {
        return;
    }
    std::map<int, int>::iterator it =
        G_CDataManager()->m_mapA928.find(static_cast<int>(item));
    if (it == G_CDataManager()->m_mapA928.end())   // ORIG call eq;jne 早退形
    {
        return;
    }
    const unsigned int flag = static_cast<unsigned int>(it->second);
    setItemCheckFlag(user, item, flag);
}
