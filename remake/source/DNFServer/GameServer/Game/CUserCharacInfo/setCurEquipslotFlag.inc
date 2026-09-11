void CUserCharacInfo::setCurEquipslotFlag(char flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_curEquipslotFlag = flag;
    }
}