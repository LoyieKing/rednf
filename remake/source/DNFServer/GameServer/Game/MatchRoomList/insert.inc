bool CWaitingRoomList::insert(IMatch* room)
{
    return m_roomSet.insert(room).second;
}