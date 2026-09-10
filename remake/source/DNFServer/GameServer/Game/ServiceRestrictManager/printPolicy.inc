void ServiceRestrictManager::printPolicy(RestrictCategory::Enum category)
{
    cMyTrace trace1(__PRETTY_FUNCTION__,
                    0x1ef, 9, true, true);
    trace1("[!] Service Restrict List ... ");
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        it->second->printRestrict();
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
                 0x1f3, 9, true, true)("[!] Service Restrict List ... End");
    }
