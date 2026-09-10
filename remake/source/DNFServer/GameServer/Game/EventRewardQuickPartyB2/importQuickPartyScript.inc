bool importQuickPartyScript(const char* path, QuickPartyData* data)
{
    if (!loadRDARScriptFile("", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    token.clear();                                    // ORIG：C1 后紧跟 Ss::clear
    std::string value;
    value.clear();
    bool ok = false;
    int lv = 0;                                       // ORIG -0xf8：全函数复用
    LevelData entry;                                  // ORIG -0x100：函数级，两节复用

    int id;

    while (true)
    {
        if (!ScanType(token, true))
            break;                                    // 主扫描失败 → false

    re_lookup:
        static std::map<const char*, int, less_TCHAR> s_lookup;
        static SpinLock s_initLock;
        static bool s_inited = false;

        if (s_inited)
        {
            LookupMap::const_iterator it = s_lookup.find(token.c_str());
            id = (it != s_lookup.end()) ? it->second : -1;
        }
        else
        {
            s_initLock.enter();
            if (s_inited)
            {
                s_initLock.leave();
                goto re_lookup;                       // 双检命中：重查表
            }
            id = 0;                                   // 首遍哨兵 → 注册链
        }

        switch (id)
        {
        case 0:                                       // 首遍注册哨兵（与 0x3e 同块）
        case QPSEC_QUICK_PARTY_END:                   // [/quick party] → 成功
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[/quick party]", (int)QPSEC_QUICK_PARTY_END));
            }
            else
            {
                return true;
            }
            // 首遍：继续登记下一段

        case QPSEC_UNABLE:                            // 单扫描：值即入列元素
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[unable to quickparty]", (int)QPSEC_UNABLE));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_dungeonIdxList.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_LEVEL_SECTION:                     // 单扫描：值即入列元素
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[level section]", (int)QPSEC_LEVEL_SECTION));
            }
            else
            {
                for (;;)
                {
                    lv = ScanInt(&ok);
                    if (!ok)
                        break;
                    data->m_vec00.push_back(lv);
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_BASIC_REWARD:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[basic reward]", (int)QPSEC_BASIC_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/basic reward]")
                        break;

                    if (token == "[increase gold card]")
                    {
                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            data->m_basicReward.m_levelDatas.push_back(entry);
                        }
                    }
                    else if (token == "[hell ratio]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;                    // ORIG：退出节回主循环
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field18 = lv;   // +0x30
                        lv = ScanInt((bool*)0);
                        data->m_basicReward.m_field1c = lv;   // +0x34
                        LevelDataHell entryHell;
                        for (;;)                      // 门扫描弃值，体读两 int
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entryHell.m_field0 = lv;
                            lv = ScanInt((bool*)0);
                            entryHell.m_value4 = lv;
                            data->m_basicReward.m_hellDatas.push_back(entryHell);
                        }
                    }
                    // 其余令牌：忽略并继续节内扫描
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[random reward]", (int)QPSEC_RANDOM_REWARD));
            }
            else
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                        break;
                    if (token == "[/random reward]")
                        break;

                    if (token == "[reward]")
                    {
                        ok = ScanStr(&value);
                        if (!ok)
                            break;
                        LevelDataRandomReward lrr;    // ctor：+0=0xb、串/向量清空
                        lrr.m_field0 = getRandomBuffTypeByName(value);
                        ScanStr(&lrr.m_name);         // 返回值被忽略（ORIG 同）

                        for (;;)                      // 门扫描弃值，体读「档」
                        {
                            lv = ScanInt(&ok);
                            if (!ok)
                                break;
                            lv = ScanInt((bool*)0);
                            entry.m_value4 = lv;
                            ok = ScanStr(&value);
                            entry.m_field0 = (ok && value == "%") ? 1 : 0;
                            lrr.m_levelDatas.push_back(entry);
                        }

                        // ORIG：value_type 引用构造（非 make_pair）
                        data->m_randomRewards.insert(
                            std::map<RandomBuffType, LevelDataRandomReward>::value_type(
                                lrr.m_field0, lrr));
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_RANDOM_REWARD_PROB:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[random reward prob]", (int)QPSEC_RANDOM_REWARD_PROB));
            }
            else
            {
                for (;;)
                {
                    ok = ScanStr(&value);
                    if (!ok)
                        break;
                    const int idx = (int)getRandomBuffTypeByName(value);
                    if (idx > 10)                     // ORIG jle 实证上界
                        continue;
                    for (int i = 0; i < 7; ++i)
                    {
                        lv = ScanInt((bool*)0);
                        data->m_probs[idx][i] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        case QPSEC_REQUIRE_PARTY_NUM:
            if (!s_inited)
            {
                s_lookup.insert(LookupMap::value_type(
                    "[require party num]", (int)QPSEC_REQUIRE_PARTY_NUM));
            }
            else
            {
                while (ScanStr(&value))
                {
                    int dungeonType = -1;
                    if (value == "[normal]")       dungeonType = 0;
                    else if (value == "[ancient]") dungeonType = 1;
                    else break;

                    if (!ScanStr(&value))
                        break;

                    int diff = -1;
                    if      (value == "[easy]")     diff = 0;
                    else if (value == "[medium]")   diff = 1;
                    else if (value == "[hard]")     diff = 2;
                    else if (value == "[ultimate]") diff = 3;
                    else break;

                    if (!ScanInt(&lv))
                        break;

                    // ORIG 双边界检查（js/jg 实证）：type∈{0,1}、diff∈{0..4}
                    if (dungeonType >= 0 && dungeonType <= 1 &&
                        diff >= 0 && diff <= 4)
                    {
                        data->m_minCounts[dungeonType][diff] = lv;
                    }
                }
                break;
            }
            // 首遍：继续登记下一段

        default:                                      // 未知令牌（id==-1）：跳过
            break;
        }

        if (!s_inited)
        {
            s_initLock.leave();
            s_inited = true;
            goto re_lookup;                           // 首遍登记完 → 重查表
        }
    }

    return ok;
