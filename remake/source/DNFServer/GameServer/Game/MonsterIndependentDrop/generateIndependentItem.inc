void generateIndependentItem(
    const std::vector<stMonsterIndependentDrop_t>& drops,
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out)
{
    const unsigned char jobIdx = (unsigned char)ref.m_rateSlot;     // movzbl 0x36
    const unsigned char chanByte = (unsigned char)ref.m_field34;    // movzbl 0x34
    // ORIG 0x83498bc `movzbl 0x3e(%eax),%eax` ⇒ +0x3e 独立字节视图（旧
    // `(unsigned char)m_goldFactor` 恒产 movzwl 0x3e，P196 字段宽度）。
    const unsigned char groupIdx = ref.m_field3e_byte;              // movzbl 0x3e

    // ORIG 0x83498d0/0x83498e5 在循环初始化区各求值一次 begin/end（P13 缓存
    // end，非每次迭代重调）。
    std::vector<stMonsterIndependentDrop_t>::const_iterator it = drops.begin();
    const std::vector<stMonsterIndependentDrop_t>::const_iterator endIter =
        drops.end();
    for (; it != endIter; ++it)
    {
        stMonsterIndependentDrop_t drop(*it);                       // copy ctor

        // ---- 1) 范围过滤 ----
        if (drop.m_rangeMin != 0 || drop.m_rangeMax != 0)
        {
            if (ref.m_field25)                                      // movzbl/test
            {
                if (ref.m_field2c < drop.m_rangeMin)                // jl → skip
                    continue;
                if (!(ref.m_field2c <= drop.m_rangeMax))            // jle 取反
                    continue;
            }
            else
            {
                if (drop.m_rangeMin > (int)chanByte)                // jg → skip
                    continue;
                if (drop.m_rangeMax < (int)chanByte)                // jl → skip
                    continue;
            }
        }

        // ---- 2) 掉率衰减控制（命中计数器先清零，ORIG -0x34 槽）----
        int hitCount = 0;
        independentItemRateControl(drop, ref);

        // ---- 3) premium 掉率加成 ----
        // ORIG 0x83499b6 `movzbl 0x3c(%eax),%eax` ⇒ +0x3c 独立字节视图（P196）。
        if (drop.m_rate[10] > 0 &&
            ref.m_premiumDropRate > 0 &&
            ref.m_field3c_byte == 3)
        {
            // imul + 0x51eb851f/sar5 有符号除 100，结果再经 fildl/fistpl 恒等
            // 往返（ORIG 0x83499f8/0x8349a1a）⇒ 源码含 (int)(float) 转换。
            drop.m_rate[jobIdx] = (int)(float)(
                drop.m_rate[jobIdx] * (ref.m_premiumDropRate + 100) / 100);
        }

        // ---- 4) 倍率放大并封顶 ----
        if (ref.m_field25 && ref.m_field30 > 1)
        {
            // ORIG 0x8349a49 / 0x8349a75 两次独立 imul（无 product 局部槽，P0：
            // 原码无具名局部 ⇒ 比较与赋值各写一遍完整表达式）；两侧均先装 rate
            // 再装 field30（0x8349a4d/0x8349a57）⇒ rate 为左操作数。
            if (drop.m_rate[jobIdx] * ref.m_field30 > 0xf423f)      // 1000000
                drop.m_rate[jobIdx] = 0xf4240;
            else
                drop.m_rate[jobIdx] = drop.m_rate[jobIdx] * ref.m_field30;
        }

        // ---- 5) 生成数量 ----
        int genCount = 0;
        if (drop.m_itemIdx == 0 && drop.m_rate[10] == 0)
        {
            // ORIG 0x8349aaf `movzbl g; sub $1; add $4; mov -0xc0(%ebp,%eax,4)`
            // ⇒ 绝对地址 drop+0x14+4g = m_rate[groupIdx+4]（m_rate 基址 +0x04）。
            // 旧 `groupIdx+3` 读 drop+0x10+4g，少 1 个元素 = 真语义 bug。
            genCount = drop.m_rate[groupIdx + 4];
        }
        else
        {
            // ORIG 0x8349ac8 仅发 j=0（hitCount 已在 0x834997b 调用
            // rateControl 前清零一次，else 分支不重复清零）。
            for (int j = 0; drop.m_rate[groupIdx + 4] > j; ++j)
            {
                if (ref.m_field24)                                  // movzbl/test
                    ++hitCount;
                else if (drop.m_rate[jobIdx] >
                         get_rand_int(0xf4240))                     // 1000000
                    ++hitCount;                                     // setg
            }
            genCount = std::min(hitCount, drop.m_rate[9]);          // _ZSt3minIi
        }

        // ---- 6) 物品生成 ----
        Inven_Item item;                                            // Inven_ItemC1Ev
        if (drop.m_rate[10] != 0)
        {
            if (ref.m_field25 &&
                drop.m_levelFilter >= 0 &&                          // js → 不过滤
                ref.m_field28 != drop.m_levelFilter)
                continue;

            for (int k = 0; k < genCount; ++k)
            {
                const int roll = get_rand_int(drop.m_rate[10]);
                // ORIG 0x8349bad 段：begin() 返回 iterator 经 const_iterator 转换
                // 构造（P96 翻案——ORIG 侧确有转换临时）；循环体用 operator*（deEv）
                // 而非 operator->（P167 新判据），并整对拷贝到 8 字节局部
                // -0x40/-0x3c（0x8349be7..0x8349bef）。
                for (std::vector<std::pair<int, int> >::const_iterator p =
                         drop.m_pairList.begin();
                     p != drop.m_pairList.end(); ++p)
                {
                    const std::pair<int, int> pr = *p;
                    if (pr.second > roll)
                    {
                        item.m_addInfo = pr.first;                  // +0x02 ← first
                        break;                                      // 未命中沿用旧值
                    }
                }
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_1, item, 1);
                out.push_back(item);
            }
            continue;
        }
        else
        {
            if (ref.m_field25 &&
                drop.m_levelFilter >= 0 &&
                ref.m_field28 != drop.m_levelFilter)
                continue;

            item.m_addInfo = drop.m_itemIdx;                        // +0x02 ← +0x00
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, 1);
            if (item.m_addInfo == 0 && drop.m_rate[10] == 0)
            {
                item.m_addInfo2 = genCount;                         // +0x07 ← count
                out.push_back(item);
            }
            else
            {
                for (int k = 0; k < genCount; ++k)
                    out.push_back(item);
            }
        }
        // 循环尾：局部 copy/item 析构由编译器生成（与 ORIG 8349d63 一致）。
    }
