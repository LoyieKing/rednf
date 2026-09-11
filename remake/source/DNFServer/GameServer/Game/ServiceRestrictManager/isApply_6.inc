// ORIG 0x0816e648：map 无该类别→true；否则策略 isApply()（虚槽 +0x10）。
bool ServiceRestrictManager::isApply(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
        return it->second->isApply();
    return true;
}