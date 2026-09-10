CreateCharacterRestrictPolicy::CreateCharacterRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_2)
{
    m_apply = false;          // ORIG 0816de0d
    m_svc.Init();             // ORIG 0816de06
    for (int i = 0; i <= 2; ++i)      // ORIG 0816de34 cmpl $0x2/setle（3 条）
    {
        m_svc.push_back(static_cast<short>(i));   // ORIG push_back(short&&)
    }
