SecurityRestrictPolicy::SecurityRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_3)
{
    m_apply = false;
    m_svc.Init();
    for (int i = 0; i <= 1; ++i)
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
