// ORIG W 0x084ece20：单字节写 this+0x703d8（m_breakAway.m_field14 bool 直写）。
void CUser::setCharacLinkMessageFlag(bool flag)
{
    m_breakAway.m_field14 = flag;
}