bool ServiceRestrictManager::isRestricted(CUser* user,
                                          RestrictCategory::Enum category, int v)
{
    if (user == 0)
        return false;
    if (reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData())->getMobileAuth())
        return false;
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
        return it->second->isApply(v);
    }
    return true;
