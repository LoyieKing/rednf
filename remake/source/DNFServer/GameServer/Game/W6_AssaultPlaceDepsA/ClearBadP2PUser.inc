// ---------------------------------------------------------------------------
// ORIG T 0x82e949c (size 0x65)：ClearBadP2PUser()
// 反复求 min/max 并按 min 清坏连接槽，直至区间收敛
// ---------------------------------------------------------------------------
void CAssaultPlace::ClearBadP2PUser()
{
    int minX = 0;
    int maxX = 0;
    for (int i = 0; i < 8; ++i) {
        _GetConnectP2PMaxMinPoint(minX, maxX);
        if (minX == maxX)
            return;          // asm cmp max,min je end
        _ClearConnectP2PRelative(maxX);   // ORIG -0x14=maxX 槽（第三实参引用）
    }
}