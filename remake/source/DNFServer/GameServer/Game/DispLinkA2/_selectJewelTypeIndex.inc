int Dispatcher_DisJointAvatar::_selectJewelTypeIndex(
    stAvatarEmblemInfo_t const* jewelSocketData) const
{
    int typeCounts[5] = {0, 0, 0, 0, 0};   // -0x3c..-0x2c：各纹章类型计数
    int cumul[5]      = {0, 0, 0, 0, 0};   // -0x50..-0x40：累计概率分布
    int total = 0;                         // -0x28
    int totalWeight = 0;                   // -0x24
    int picked;                            // -0x20
    int i;                                 // -0x1c
    int idx;                               // -0x18
    int j;                                 // -0x14
    int weight;                            // -0x10
    int k;                                 // -0xc

    for (i = 0; i < 5; ++i)
    {
        // stAvatarEmblemInfo_t pack(1)：5×{ushort emblemSocketType_@+i*6, int emblemItemIdx_}
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffef)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffff)
            continue;
        // ORIG：idx = _getJewelTypeIndex(type)（getAvatarEmblemTypeIdx），-1 → continue
        idx = _getJewelTypeIndex(
            jewelSocketData->stEmblemSocket[i].emblemSocketType_);
        if (idx == -1)
            continue;
        // [b28d-solo-DispLinkA2] P38 栈数组变体：`arr[i]=arr[i]+1` 在 4.4 恒双读下标
        // （mov idx,%eax; mov idx,%edx; …）；`arr[i]++` 复现 ORIG 单读
        // （mov idx,%eax; mov arr(,%eax,4),%edx; add $1; mov %edx,arr(,%eax,4)）。
        typeCounts[idx]++;
        total = total + 1;
    }

    if (total == 0)
        return -1;

    for (j = 0; j < 5; ++j)
    {
        // [b28d-solo-DispLinkA2] ORIG 先 idiv 后取数组成员（右操作数先求值）；
        // 交换乘法书写序令 4.4 同序（P23/P75：整乘交换律语义等价）。
        weight = (10000 / total) * typeCounts[j];
        cumul[j] = weight + totalWeight;
        totalWeight = totalWeight + weight;
    }

    picked = get_rand_int(totalWeight);         // ORIG 0x086b1b87 get_rand_int(int)

    for (k = 0; k < 5; ++k)
    {
        if (picked < cumul[k])
            return k;
    }
    return -1;
