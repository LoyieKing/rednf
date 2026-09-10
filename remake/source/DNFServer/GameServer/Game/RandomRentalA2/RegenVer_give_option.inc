bool CRandomOptionItemHandle::RegenVer_give_option(
    unsigned long itemIdx, ENUM_RARITY rarity, int level, int group, int grade,
    RandomOption* opts, RegenerationROI* roi, int optType)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 506/486（ORIG 0x085f1bb0）：P0/P48 四处
    // GetRandomOption 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    unsigned char outGrade = 0;
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, grade, outGrade);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1d8, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)rarity, level, grade, (int)outGrade, err);
        return false;
    }

    int quantity = 0;
    if (optType == 0x65)
    {
        err = RegenVer_get_option_quantity(rarity, grade, outGrade, level,
                                           quantity, roi);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "RandomOption.cpp",
                "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
                "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
                " RegenerationROI*, int)",
                0x1e2, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
                (int)rarity, grade, (int)outGrade, level, quantity, err);
            return false;
        }
    }
    else
    {
        err = _get_option_quantity(rarity, grade, outGrade, level, quantity);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "RandomOption.cpp",
                "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
                "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
                " RegenerationROI*, int)",
                0x1ea, "_get_option_quantity (%d,%d,%d,%d,%d) error_line(%d)",
                (int)rarity, grade, (int)outGrade, level, quantity, err);
            return false;
        }
    }

    int groupId = 0;
    err = _choose_option_group(rarity, grade, group, level, outGrade, groupId);
    if (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1f2, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, grade, group, level, (int)outGrade, groupId, err);
        return false;
    }

    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 4 处独立调 GetRandomOption
    // （mapf4 find/end + map160 find/end），无具名 script 槽。
    DifferentWeightValue dwTmp;  // ORIG 未使用临时（O0 保留 ctor call）
    unsigned short dwRaw =
        (unsigned short)(((unsigned short)(unsigned char)rarity) |
                         ((unsigned short)outGrade << 8));
    std::map<short, DifferentWeightValue>::const_iterator dw =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.find((short)dwRaw);
    if (dw == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x1fe, "different_weight_ (%d,%d) ",
            (int)(unsigned char)rarity, (int)outGrade);
        return false;
    }
    int dwA = dw->second.m_field0;
    int dwB = dw->second.m_field4;

    std::map<int, OptionGroupValue>::const_iterator ogIt =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.find(groupId);
    if (ogIt == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            "bool random_option::CRandomOptionItemHandle::RegenVer_give_"
            "option(itemIndex_t, ENUM_RARITY, int, int, int, RandomOption*,"
            " RegenerationROI*, int)",
            0x206, "option_group_ (%d) ", groupId);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;
    // [Lane13 视图 cast 反模式] 同 give_option：字节视图 → 权威头具名字段直访问。

    for (int slotI = 0; slotI < quantity && slotI <= 2; ++slotI)
    {
        unsigned long bound = (unsigned long)remaining;
        int pickN = m_rand[9].randInt(bound);

        // [Lane13] P278/P221：chooser 匿名临时，get_value 作用 for_each 返回值。
        unsigned int sel =
            std::for_each(groupVec.begin(), groupVec.end(),
                          CRandomList<unsigned int>((unsigned int)pickN))
                .get_value();
        if (sel < groupVec.size())
        {
            OptionGroup og = groupVec[sel];
            opts->m_fields[slotI].m_field0 = (unsigned char)og.m_type0;
            remaining -= og.m_weight4;

            // [b28d-solo-RandomRentalA2 P314/P50] 同 give_option。
            std::vector<OptionGroup>::iterator victim;
            victim = groupVec.begin() + sel;
            groupVec.erase(victim);

            unsigned char v1;
            unsigned char v2;
            if (optType == 0xc9)
            {
                v1 = (unsigned char)roi->RegenVer_get_option_value(dwA, dwB);
                v2 = (unsigned char)roi->RegenVer_get_option_value(dwA, dwB);
            }
            else
            {
                v1 = (unsigned char)_get_option_value(dwA, dwB);
                v2 = (unsigned char)_get_option_value(dwA, dwB);
            }
            opts->m_fields[slotI].m_field1 = v1;
            opts->m_fields[slotI].m_field2 = v2;
        }
    }

    opts->m_seed9.m_field =
        (unsigned char)((opts->m_seed9.m_field & 0xfc) | (outGrade & 3));
    unsigned long seedBound = MAX_RANDOM_SEED;
    int seedRoll = m_rand[10].randInt(seedBound);
    opts->m_seed9.m_field =
        (unsigned char)((opts->m_seed9.m_field & 3) | ((seedRoll & 0x3f) << 2));

    return true;
