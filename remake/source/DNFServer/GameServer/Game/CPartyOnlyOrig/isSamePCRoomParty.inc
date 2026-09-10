bool CParty::isSamePCRoomParty()
{
    int firstRoomNo = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))     // ORIG 正向判定+真路径 fall-through（技巧17/19）
        {
            int roomNo = m_member[i].m_pUser->GetPCRoomNo();
            if (roomNo == 0)
            {
                return false;
            }
            if (firstRoomNo == 0)
            {
                firstRoomNo = roomNo;
            }
            else if (firstRoomNo != roomNo)
            {
                return false;
            }
        }
    }
    return true;
