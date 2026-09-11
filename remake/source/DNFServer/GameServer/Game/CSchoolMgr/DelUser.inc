void CSchoolMgr::DelUser(CUser* user)
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserOut(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second -= 1;
        if (it->second == 0)
        {
            m_schoolUserCount.erase(it);
            m_schoolArea.DelSchoolArea(user->GetSchoolNo());
        }
    }
}