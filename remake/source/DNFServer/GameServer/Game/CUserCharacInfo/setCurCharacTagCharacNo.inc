void CUserCharacInfo::setCurCharacTagCharacNo(unsigned int no)
{
    if (m_selected)
    {
    enableSaveCharacManageInfo();
    CUR->m_tagCharacNo = no;
    }
}