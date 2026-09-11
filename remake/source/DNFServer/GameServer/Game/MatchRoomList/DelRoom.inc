bool CLeagueMatchRoomList::DelRoom(IMatch* room)
{
    std::map<IMatch*, int>::iterator it = m_roomMap.find(room);
    if (it != m_roomMap.end()) {
        m_roomMap.erase(it);
        return true;
    }
    return false;
}