// ORIG 0x0816e54a：find 命中则策略 beginPolicy(str)；随后 printPolicy + updatePolicy。
void ServiceRestrictManager::beginPolicy(RestrictCategory::Enum category,
                                         std::string const& str)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e5a7：虚调用 vtable+0x8（槽2）= beginPolicy(string)。
        it->second->beginPolicy(str);
    }
    printPolicy(category);
    updatePolicy(category);
}