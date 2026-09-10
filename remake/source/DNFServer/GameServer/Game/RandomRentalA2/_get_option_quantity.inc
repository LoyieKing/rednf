int CRandomOptionItemHandle::_get_option_quantity(ENUM_RARITY rarity, int level,
                                                  int grade, int optType,
                                                  int& outQuantity)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 212/220；ORIG 0x085f25b0）：P0/P48 五处
    // GetRandomOption 内联 + w 标量局部已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 5 处独立调 GetRandomOption
    // （两次 find + 两次 end + 循环内 m_veca8 基址），无具名 script 槽。
    // [SweepRandomRental-2 P82] ORIG 0x085f25b8..0x085f25cb 为四条 disp 写、
    // 键地址每次 `lea -0x54(%ebp),%edx` 重取 ⇒ 具名 4 字节 POD 视图
    // （数组 + `*reinterpret_cast<int*>` 使 4.4 把 &key 缓存进 %ebx）。
    union OptionQuantityKey
    {
        struct
        {
            unsigned char m_rarity;    // +0x0
            unsigned char m_level;     // +0x1（0xff 或 level）
            unsigned char m_grade;     // +0x2
            unsigned char m_optType;   // +0x3
        } m_parts;
        int m_key;                     // 4 字节键视图（find 直传左值）
    };
    OptionQuantityKey key;
    key.m_parts.m_rarity = (unsigned char)rarity;
    key.m_parts.m_level = 0xff;
    key.m_parts.m_grade = (unsigned char)grade;
    key.m_parts.m_optType = (unsigned char)optType;

    // [b28d-solo-RandomRentalA2 P0/P235] ORIG find 直传 &key（无 mapKey
    // 拷贝/无 memcpy 调用）；union 视图左值直绑 const&。
    // [b28d-solo-RandomRentalA2 P96/P270 反向] ORIG 走 const 版容器访问
    // （p167 MISSING `_ZNKSt3map...4find`/`_ZNKSt23_Rb_tree_const_iterator...eq`）
    // ⇒ 经 const 指针访问成员 + const_iterator。
    std::map<int, OptionQuantityValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
            key.m_key);
    if (it == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
    {
        key.m_parts.m_level = (unsigned char)level;
        // [SweepRandomRental-2 P293 反向] ORIG 0x085f262b..0x085f2679 先
        // GetRandomOption+find（→-0x68）再 GetRandomOption+end（→-0x4c）；
        // 单表达式 `find()==end()` 令 4.4 反序成 end 先 ⇒ 具名 const_iterator。
        // [b28d-solo-RandomRentalA2 P171] 第二次 find 不回写 it（后续仍用首次）。
        std::map<int, OptionQuantityValue>::const_iterator retryIt =
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.find(
                key.m_key);
        if (retryIt ==
            reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapb4.end())
        {
            return 0x26c;
        }
    }

    if (it->second.m_maxB == 0)
    {
        return 0x270;
    }

    std::vector<unsigned int> weights;
    unsigned int total = 0;
    // [SweepRandomRental-2] ORIG 0x085f26db/0x085f271d 为 32 位槽
    // （`mov %eax,-0xc`/`addl $0x1,-0xc`）⇒ 循环变量 int（short 形产 16 位槽）。
    // [SweepRandomRental-2] ORIG 0x085f26c1 `movl $0x0,-0x14`（w 在循环外
    // 声明并初始化 0，循环内仅赋值）⇒ 循环内声明会少一条初始化。
    unsigned int w = 0;
    for (int i = it->second.m_minA - 1; i < it->second.m_maxB; ++i)
    {
        // ORIG：每轮一次 operator[]，值存 -0x14 后 push_back / total 各读一次
        // （P0 有槽往返 ⇒ 具名标量局部 w）。
        w = reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                ->m_veca8[i]
                .m_weight4;
        // [b28d-solo-RandomRentalA2 P315/P319] ORIG push_back 选右值重载
        // `_ZNSt6vectorIjSaIjEE9push_backEOj` ⇒ 实参写 cast 成 prvalue。
        weights.push_back((unsigned int)w);
        total += w;
    }

    if (weights.empty())
    {
        return 0x27d;
    }

    if (weights.size() == 1)
    {
        outQuantity = it->second.m_minA;
        return 0;
    }

    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r = m_rand[5].randInt((unsigned long)total);
    // [Lane13] P278/P221：ORIG 085f29?? 把 CRandomList 构到 -0x30（浅）、
    // for_each 返回值 sret -0x40 上直调 get_value（-0x40），非具名 chooser。
    // [Lane13] P211：ORIG 085f29f7 `lea (%ebx,%eax,1),%eax; mov %eax,%edx` ——
    // RHS 为 unsigned 表达式、目标 int ⇒ 异型触发 eax→edx 中转；突兀 `(int)` cast
    // 会把 RHS 归为 int（同型）而丢掉中转 ⇒ 删 cast 保留 unsigned 语义。
    outQuantity =
        std::for_each(weights.begin(), weights.end(),
                      CRandomList<unsigned int>((unsigned int)r))
            .get_value() +
        it->second.m_minA;
    return 0;
