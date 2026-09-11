// ORIG T 0x085b687c（isSamePCRoomParty.md）：首个成员 PCBang 号缓存后逐一
// 相同；任一为 0 或不一致即 false，全部一致 true。
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
}