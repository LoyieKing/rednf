int Dispatcher_CompoundEmblem::_selectJewelTypeIndex(
    unsigned int count, const unsigned short* sockets) const
{
    // b28d-40: N82/N93 封顶——帧 0x78 vs 0x74+push ebx + 具名数组槽序差，语义闭合。
    // ORIG 0x08220a60。
    int socketCounts[5] = {0, 0, 0, 0, 0};   // 各类型计数（ORIG local_54[6..10]）
    int accums[5]        = {0, 0, 0, 0, 0};   // 各类型累计权重（local_54[1..5]）
    int total            = 0;                 // 权重总和（local_54[0]）
    int cumulative       = 0;                 // 累计（local_54[0xb]）

    for (unsigned int i = 0; i < count; ++i)
    {
        if (sockets[i] != 0)
            getAvatarEmblemTypeIdxForCompountEmblem(sockets[i], socketCounts, total);
    }
    if (total != 0)
    {
        for (int j = 0; j < 5; ++j)
        {
            int weight = socketCounts[j] * (int)(10000 / total);
            accums[j]  = weight + cumulative;
            cumulative += weight;
        }
        int r = get_rand_int(cumulative);
        for (int k = 0; k < 5; ++k)
        {
            if (r < accums[k])
                return k;
        }
    }
    return -1;
}
