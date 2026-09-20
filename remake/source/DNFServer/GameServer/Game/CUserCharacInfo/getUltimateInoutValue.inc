unsigned char CUserCharacInfo::getUltimateInoutValue(char index)
{
    // [b28] 值 cast 伪差已消（m_selView 直读），本函数 AE。
    return m_selected->m_ultimateInout[index];
}