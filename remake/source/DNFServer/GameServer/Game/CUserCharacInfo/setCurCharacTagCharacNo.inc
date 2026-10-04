void CUserCharacInfo::setCurCharacTagCharacNo(unsigned int no)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    m_selected->m_tagCharacNo = no;
    }
}