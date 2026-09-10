int CGameManager::GetWarRoomIdx(WarRoom* room)
{
    int idx = m_warRoomPool.GetIndex(room);
    return idx;
