void CSchoolMgr::AddUser(CUser* user)
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserIn(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second += 1;
    }
    else
    {
        m_schoolUserCount.insert(std::make_pair(user->GetSchoolNo(), 1));
        m_schoolArea.AddSchoolArea(user->GetSchoolNo());
    }
}