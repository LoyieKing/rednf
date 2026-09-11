void CUserCharacInfo::setMemberDungeonFlag(bool flag)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_memberDungeonFlag = flag;
    }
}