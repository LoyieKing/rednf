void CUser::SetProgLogout()
{
    m_progLogout = 1;   // 字段已 bool 化（ORIG IsProgLogout@0x82a6796 movzbl 直返）；&-cast 反而多余
}