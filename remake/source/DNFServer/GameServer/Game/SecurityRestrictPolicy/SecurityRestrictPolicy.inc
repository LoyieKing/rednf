// ORIG 0x0816da50：基类 ctor(Enum_3) + 清 m_apply + ServicePolicy::Init +
// push_back(1) × 2（setle 循环 i<=1）。
SecurityRestrictPolicy::SecurityRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_3)
{
    m_apply = false;
    m_svc.Init();
    for (int i = 0; i <= 1; ++i)
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
}