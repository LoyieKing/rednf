int CGameManager::GetPvpIdx(PvP_Room* room)
{
    int idx = m_pvpRoomPool.GetIndex(room);
    return idx;
}