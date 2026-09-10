void CDeathMatchBattleMgr::OnJoinRoom(int roomIdx)
{
    if (roomIdx < 0 || roomIdx > 7)
        return;
