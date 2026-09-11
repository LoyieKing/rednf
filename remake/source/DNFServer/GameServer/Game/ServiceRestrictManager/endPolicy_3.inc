// ORIG 0x0816e5de：find 命中则策略 endPolicy()（虚槽 +0xc）。
void ServiceRestrictManager::endPolicy(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        it->second->endPolicy();
    }
}