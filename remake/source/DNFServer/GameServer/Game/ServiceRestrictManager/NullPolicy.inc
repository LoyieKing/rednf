// ---- NullPolicy（ORIG 0x0816d5aa 区，全部空/恒真）----
NullPolicy::NullPolicy() : InterfaceServicePolicy(RestrictCategory::ENUM_0)
{
    m_apply = false;          // ORIG 0816d5d1 movb $0,0x4
    m_svc.Init();             // ORIG 0816d5de call ServicePolicy::Init
}