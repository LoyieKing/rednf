online_preliminary::COnlinePreliminaryTeamMgr::FindTeam(int teamId)
{
    std::map<int, COnlinePreliminaryTeam*>::iterator it = m_teamMap.find(teamId);
    // ORIG je→zero-island、成功臂 fall-through ⇒ 正极性 if（P47 镜像律试形）
    if (it != m_teamMap.end())
        return it->second;
    return 0;
