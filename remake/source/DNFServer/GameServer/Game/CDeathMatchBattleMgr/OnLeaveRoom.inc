void CDeathMatchBattleMgr::OnLeaveRoom(int roomIdx)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
