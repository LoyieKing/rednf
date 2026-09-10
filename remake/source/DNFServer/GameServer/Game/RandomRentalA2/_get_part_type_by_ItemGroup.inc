int CRandomOptionItemHandle::_get_part_type_by_ItemGroup(
    int limit, const std::vector<int>& itemGroups) const
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 192/180（ORIG 0x085f11a4）：P0/P48 script
    // 内联已消缓存局部；残余循环形态/槽序。旧判词：
    // （P183 族）。语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption。
    std::map<int, PartSelectonValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map30.find(limit);
    // [SweepRandomRental-2] ORIG 0x085f11fb `je 085f1415`（it==end 直跳函数尾
    // 共享 `mov $0x0,%esi` 出口）⇒ 正向包裹 + 单一尾 return 0（早返形多发
    // `mov $0x0,%ebx; jmp` 三条）。
    if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map30.end())
    {
        std::vector<int> weights(itemGroups.size(), 0);
        unsigned int total = 0;

    // [SweepRandomRental-2 P183] ORIG 0x085f126a/0x085f1323 每轮重取
    // it->second.m_vec10（ptEv + lea 0x14）、迭代器循环（begin/end/ppEv）；
    // 0x085f12d6 再 ptEv 取 m_weight 写入 weights[i]，0x085f12e6 二次
    // operator[] 读回 weights[i] 累加（无 vec 引用/索引局部）。
        for (std::vector<PartSelectonField>::const_iterator v =
                 it->second.m_vec10.begin();
             v != it->second.m_vec10.end(); ++v)
        {
            for (std::size_t i = 0; i < itemGroups.size(); ++i)
            {
                if (v->m_partType == itemGroups[i])
                {
                    weights[i] = (unsigned int)v->m_weight;
                    total += (unsigned int)weights[i];
                    break;
                }
            }
        }

        // [b28d-solo-RandomRentalA2 P0] bound 为 randInt 匿名临时（同族范式）。
        unsigned int r = (unsigned int)m_rand[2].randInt((unsigned long)total);

        unsigned int acc = 0;
        for (std::size_t i = 0; i < weights.size(); ++i)
        {
            acc += weights[i];
            if (r < acc)
            {
                return itemGroups[i];
            }
        }
    }
    return 0;
