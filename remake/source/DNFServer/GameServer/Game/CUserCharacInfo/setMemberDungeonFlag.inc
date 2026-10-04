void CUserCharacInfo::setMemberDungeonFlag(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_memberDungeonFlag = flag;
    }
}