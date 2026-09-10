bool CRandomOptionItemHandle::change_option(unsigned long itemIdx, ENUM_RARITY rarity,
                                            int level, int grade, int group,
                                            int slot, RandomOption* opts)
{
    if (slot < 0 || slot > 2)
    {
        return false;
    }


    unsigned char optType = 0;
    // [L31 round2] 四处预清全补反劣（406→416）⇒ 回滚。本轮只补 **err 单处**（P198① 最小集）：
    // ORIG 0x085f3352 `movl $0x0,-0x1c(%ebp)`（optType 预清后紧邻）⇒ 原码 `int err = 0;`
    // 后接赋值两句式（i/pick/sel 无预清证据）。
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, group, optType);
    // [b28d-solo-UserMiscLink] P225：ORIG 0x085f33a1 为 `cmpl $0; setne %al; test %al,%al;
    // je`（bool 物化岛）⇒ while 形复现（4.4 裸 if 恒直跳）。
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x356, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)(unsigned char)rarity, level, group,
            (int)optType, err);
        return false;
    }

    int outGroup = 0;
    err = _choose_option_group(rarity, group, grade,
                               (unsigned char)level, optType, outGroup);
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x35d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)(unsigned char)rarity, group, grade, level,
            (int)optType, outGroup, err);
        return false;
    }

    // [b28d-solo-UserMiscLink] 判词（N 封顶）：443/432、p167 CLEAR(47=47)；
    // 残差=①帧 0xc0 vs 0xd0 + 局部槽深差（optType -0x53 vs -0x65、err -0x28 vs -0x1c）
    // ⇒ N82/N95；②4.4 lea 折叠（optB[i] 字节写 vs ORIG disp 直写）；③寄存器分配。
    // 已复现：const_iterator（P96/P270 翻转 13 项 p167 差集）、begin()+sel 的 deEv/plERKi
    // （P167 判据）、setne 物化岛（P225 while 形）、union 字节键、GetRandomOption 逐点重取（P157）。
    // 不同权重表（GetRandomOption()+0xf4 = RandomOptionScript::m_mapf4）：
    // 键 = 低字节 rarity | 高字节 optType（ORIG 0x085f34d3/da 两次字节写 -0x6e/-0x6d
    // ⇒ 原码 union 字节视角）；P157：GetRandomOption() 每次重取（ORIG 四处独立 call）。
    DifferentWeightValue dwv;
    union
    {
        unsigned short m_w;
        unsigned char m_b[2];
    } dwRaw;
    dwRaw.m_b[0] = (unsigned char)rarity;
    dwRaw.m_b[1] = optType;
    std::map<short, DifferentWeightValue>::const_iterator dw =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.find(dwRaw.m_w);
    if (dw == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x369, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)optType);
        return false;
    }
    dwv = dw->second;

    // 组值表（GetRandomOption()+0x160 = m_map160）。
    std::map<int, OptionGroupValue>::const_iterator ogIt =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.find(outGroup);
    if (ogIt == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x371, "option_group_ (%d) ", outGroup);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;   // 值拷贝后 erase
    unsigned char* optB = reinterpret_cast<unsigned char*>(opts);
    // ORIG 0x085f3705：迭代器默认构造（C1Ev）在循环之前 ⇒ 先声明后赋值（P50 复活）。
    std::vector<OptionGroup>::iterator selIt;

    // 第一组已占槽（非本次 slot）按其组类型扣权重并移出候选。
    // （i 的 ORIG 预清形 0x085f3674 `movl $0x0,-0xc` 属上述已回滚组。）
    for (int i = 0; i <= 2; ++i)
    {
        if (i == slot || optB[i * 3] == 0)
        {
            continue;
        }
        // ORIG 0x085f36b7：begin 在 if 内、while 形、`(*it).m` 走 deEv（P167 新判据）。
        std::vector<OptionGroup>::iterator it = groupVec.begin();
        while (it != groupVec.end())
        {
            if ((unsigned int)(*it).m_type0 == (unsigned int)optB[i * 3])
            {
                remaining -= (*it).m_weight4;
                groupVec.erase(it);
                break;
            }
            ++it;
        }
    }

    unsigned long bound = (unsigned long)remaining;
    int pick = m_rand[9].randInt(bound);          // m_rand+0x5808 → 池[9]

    CRandomList<unsigned int> chooser((unsigned int)pick);
    std::for_each(groupVec.begin(), groupVec.end(), chooser);

    unsigned int sel = chooser.get_value();
    // [D04 轴 B/P196 逐点自校 · 已试并回滚] ORIG 0x085f383e `cmp -0x10(%ebp),%eax;
    // setbe %al; test; je <fail 块>` = `sel >= size()` guard 形；我方 `if (sel < size())`
    // 产互补 `seta`。按 Main 轴 B 协议试 guard 形（失败块后置）：**ours 442→444（+2）反劣，
    // 四数不变** ⇒ 已回滚。两 cc 均属无符号序（非 P196 符号性分叉），差异为 4.1/4.4 的
    // 条件取反 fold（ORIG 用 setbe 走 fail 岛、4.4 用 seta 直落），源形不可择 ⇒ N。
    if (sel < groupVec.size())
    {
        // ORIG 0x085f38f8：begin()+sel 再 deEv（非 operator[]；P167 判据）。
        selIt = groupVec.begin() + sel;
        optB[0xa] = (unsigned char)(*selIt).m_type0;    // 第二组槽首字节=组类型
        optB[0xb] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
        optB[0xc] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
        optB[0xd] = (unsigned char)((optB[0xd] & 0xfc) | (slot & 3));
        unsigned long seedBound = 0x40;
        int seedRoll = m_rand[10].randInt(seedBound);  // m_rand+0x61d0 → 池[10]
        optB[0xd] = (unsigned char)((optB[0xd] & 3) | ((seedRoll & 0x3f) << 2));
    }
    else
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            __PRETTY_FUNCTION__,
            0x390, "_option_group.size() %d, %d ", sel,
            (unsigned int)groupVec.size());
        return false;
    }
    return true;
