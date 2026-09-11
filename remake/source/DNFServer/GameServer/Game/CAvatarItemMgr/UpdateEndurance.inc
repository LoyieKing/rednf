void CAvatarItemMgr::UpdateEndurance(int key, int endurance)
{
    std::map<int, int>::iterator it = m_enduranceUpdateMap.find(key);
    std::map<int, int>::iterator endIt = m_enduranceUpdateMap.end();
    // [b28d-38] ORIG then 块（it->second=endurance）在函数尾、insert 块直落 ⇒
    // 否定条件写法（P222 镜像）；pair 直构替代 make_pair（省一次转换构造 call，P192）。
    if (it == endIt)
        m_enduranceUpdateMap.insert(std::pair<const int, int>(key, endurance));
    else
        it->second = endurance;
}