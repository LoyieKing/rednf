bool CWaitingRoomList::find(IMatch* room)
{
    return m_roomSet.find(room) != m_roomSet.end();
