// ORIG T 0x82e9412 _GetConnectP2PMaxMinPoint(int&, int&)
void CAssaultPlace::_GetConnectP2PMaxMinPoint(int& maxPt, int& minPt)
{
    maxPt = 0;                                  // 第 1 引用参 init 0 → 最大值
    minPt = 8;                                  // 第 2 引用参 init 8 → 最小值
    // [b28d-26] ORIG 0x82e9412：pt 声明于循环外（-0x10 槽预清 0），循环变量 i 落
    //   -0xc；Empty() 早返 continue 形产 `test %al,%al; jne <continue>`（无 xor）。
    int pt = 0;
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        pt = _GetConnectP2PPoint(i);
        if (pt < minPt)
            minPt = pt;
        if (maxPt < pt)
            maxPt = pt;
    }
}