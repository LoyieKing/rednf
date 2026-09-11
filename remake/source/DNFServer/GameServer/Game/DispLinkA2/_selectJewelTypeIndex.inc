// ============================================================================
// 1. Dispatcher_DisJointAvatar::_selectJewelTypeIndex
//    报告 docs/class_func_reports/Dispatcher_DisJointAvatar/_selectJewelTypeIndex.md
//    [b28d-solo-DispLinkA2 判词] verdict=IDENTICAL（compare_tu clean：ORIG 125/OURS 124；
//    唯一残差 = ORIG 尾部 1 条对齐 nop，§6.1 豁免）。修法两处：
//    ①`typeCounts[idx] = typeCounts[idx] + 1;` → `typeCounts[idx]++;`
//      （P38 栈数组变体：4.4 对 `a[i]=a[i]+1` 恒双读下标 `mov idx,%eax; mov idx,%edx; …`；
//       `a[i]++` 复现 ORIG 单读 `mov idx,%eax; mov a(,%eax,4),%edx; add $1; mov %edx,a(,%eax,4)`。
//       探针 /tmp/a2probe/p.cpp probeA/H 对照。）
//    ②`weight = typeCounts[j] * (10000/total);` → `(10000/total) * typeCounts[j]`
//      （ORIG 先 idiv 后取数组成员；交换书写序令 4.4 同序，P23/P75。探针 p.cpp probeD/E。）
// ============================================================================
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
}