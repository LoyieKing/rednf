AuthMobileRestrictPolicy::AuthMobileRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_1)
{
    m_apply = false;          // ORIG 0816d6f5
    m_svc.Init();             // ORIG 0816d702
    for (int i = 0; i <= 0x21; ++i)   // ORIG 0816d72f cmpl $0x21/setle（34 条）
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
