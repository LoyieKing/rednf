unsigned char CUserCharacInfo::getUltimateInoutValue(char index)
{
    // [b28] 值 cast 伪差已消（m_selView 直读），本函数 AE。
    return CUR->m_ultimateInout[index];
