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
