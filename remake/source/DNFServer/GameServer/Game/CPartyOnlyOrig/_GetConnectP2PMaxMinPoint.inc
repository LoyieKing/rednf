void CParty::_GetConnectP2PMaxMinPoint(int& maxPoint, int& minPoint)
{
    maxPoint = 0;
    minPoint = 4;
    int point = 0;                                    // -0x10
    for (int i = 0; i <= 3; i++)                      // -0xc（P2/P51：i<=3 → cmpl $0x3;setle 尾检）
    {
        // P168a①：bool callee 裸形直产 call; xor$1; test; jne→nop 岛
        if (!_checkValidUser(i))
            continue;
        point = _GetConnectP2PPoint(i);
        if (minPoint > point)                           // ORIG cmp (*min),point; jle
            minPoint = point;
        if (maxPoint < point)                           // ORIG cmp point,(*max); jge
            maxPoint = point;
    }
