bool CLeagueMatchRoomList::AddRoom(IMatch* room)
{
    m_roomMap.insert(std::make_pair(room, 1));
    std::pair<std::map<IMatch*, int>::iterator, bool> ins =
        m_roomMap.insert(std::make_pair(room, 1));
    if (!ins.second)
        ++(ins.first->second);
    return true;
