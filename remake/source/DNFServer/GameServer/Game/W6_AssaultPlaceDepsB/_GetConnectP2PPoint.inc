int CAssaultPlace::_GetConnectP2PPoint(int idx)
{
    int points = 0;                             // movl $0x0,-0x10(%ebp)
    for (int i = 0; i <= 7; ++i) {              // cmpl $0x7 … setle
        if (idx != i) {
            // [b28c-L7] P168a①：callee 改 bool ⇒ 裸形 `if (!call())` 直产
            // ORIG 0x82e93c0 `call; xor $1,%eax; test %al,%al; je`。
            if (!_IsCannotConnectP2P(idx, i))
                points++;
        }
    }
    return points;
