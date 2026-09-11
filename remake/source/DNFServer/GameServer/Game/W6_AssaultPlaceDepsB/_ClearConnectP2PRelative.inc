// ORIG T 0x82e9358 _ClearConnectP2PRelative(int)
int CAssaultPlace::_ClearConnectP2PRelative(int idx)
{
    for (int i = 2; i <= 7; ++i) {
        if (_GetConnectP2PPoint(i) == idx)
            LeaveAssaultPlace(i, true, false);
    }
    return 1;
}