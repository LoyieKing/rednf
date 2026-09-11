// ORIG T 0x0859a830（ClearBadP2PUser.md）：绝对清理两个全通槽位，再迭代
// 相对点数差于最差的相对清理。
void CParty::ClearBadP2PUser()
{
    _ClearConnectP2PAbsolute(0);
    _ClearConnectP2PAbsolute(1);

    int maxPoint = 0;
    int minPoint = 0;
    for (int i = 2; i <= 3; i++)
    {
        _GetConnectP2PMaxMinPoint(maxPoint, minPoint);
        if (maxPoint == minPoint)
        {
            break;
        }
        _ClearConnectP2PRelative(minPoint);
    }
}