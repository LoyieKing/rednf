void CUserCharacInfo::SetOneDayLetheFlag(unsigned char flag)
{
    if (m_selected)
    {
    CUR->m_oneDayLetheFlag = flag;
    }
}