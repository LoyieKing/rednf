bool ServiceRestrictManager::isApply(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
        return it->second->isApply();
    return true;
