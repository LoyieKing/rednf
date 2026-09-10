int importAmplifyItem(const char* path, stAmplifyItemScript_t& out)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：737/766，gap=-29）：
    // 本波修 4 处 const_iterator（P96 反向，MISSING const C1ERK/eq/ptE 全消）
    // + 3 处 invest key 去 const（ORIG make_pair `IRmR...`）+ 3 处 pair 直构。
    // 残差：p167 尚余 EXTRA map<string,ENUM_AMPLIFY_TYPE> C1/D1×3 + ScanInt×1
    // （ORIG 该三处 map 生命周期/读序与 4.4 折叠不同；语义等价，非缺失调用）；
    // 其余为巨件帧/槽平移 + 模板实例内联差。
    if (!loadRDARScriptFile("", path))
        return 0x1d21;

    typedef std::map<std::string, ENUM_AMPLIFY_TYPE> AmplifyTypeMap;
    AmplifyTypeMap typeMap;
    std::string token;
    std::string str;
    int value = 0;
    bool ok = false;

    while (true)
    {
        if (!ScanType(token, true))
            break;                                     // P164 配套：EOF 落点移到循环外

        if (token == "[option mapping table]")
        {
            while (ScanStr(&str))
            {
                value = ScanInt(&ok);
                if (!ok)
                    return 0x1d35;
                if (!(typeMap.insert(
                          std::make_pair(str,
                                         static_cast<ENUM_AMPLIFY_TYPE>(value)))
                          .second))
                    return 0x1d39;
            }
        }
        else if (token == "[amplification weight by level]")
        {
            out.m_weightByLevel = ScanFloat(3, &ok);
            if (!ok)
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                    0x1d40, "ERROR [amplification weight by level]");
        }
        else if (token == "[amplification rate by rarity]")
        {
            for (int i = 0;; ++i)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                if (i > 5)
                    return 0x1d4d;
                out.m_rarityProb[i] = value;
            }
        }
        else if (token == "[option data]")
        {
            int sum = 0;
            while (ScanStr(&str))
            {
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1d5e;
                const ENUM_AMPLIFY_TYPE type = it->second;
                const int prob = ScanInt(&ok);
                if (!ok)
                    return 0x1d64;
                const short probSum = static_cast<short>(prob + sum);
                const unsigned short val =
                    static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                    return 0x1d69;
                sum += prob;
                stAmplifyOptionInfo_t info;
                info.m_type = type;
                info.m_prob = static_cast<unsigned short>(probSum);
                info.m_value = val;
                out.m_options.push_back(info);
            }
        }
        else if (token == "[rarity weight]")
        {
            while (ScanStr(&str))
            {
                const float weight = ScanFloat(&ok);
                if (!ok)
                    return 0x1d78;
                const int rarity = getItemRarity(str.c_str());
                if (rarity == 6)
                    return 0x1d7b;
                out.m_rarityRate[rarity] = weight;
            }
        }
        else if (token == "[bonus weight by equipment type]")
        {
            for (int eq = 10; eq <= 21; ++eq)
            {
                out.m_bonusWeightByEquipType[eq - 10] = ScanFloat(4, &ok);
                if (!ok)
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0x1d89, "ERROR : [bonus weight by equipment type]");
            }
        }
        else if (token == "[equip level const]")
        {
            // [b28d-solo-ScriptDepsA] ORIG：本段与「未匹配任何段名」的链尾
            // 默认路径**共用同一段代码**（反编译 C 的 do-while 尾部 ScanInt
            // 实证）⇒ 单一 ScanInt 调用点（p167 EXTRA ScanInt×1 的来源）。
            goto parse_equip_level;
        }
        else if (token == "[purify material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1d98;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1d9b;
            out.m_purifyMatItem[0] = item;
            out.m_purifyMatCount[0] = count;
        }
        else if (token == "[purify only material]")
        {
            const int item = ScanInt(&ok);
            if (!ok) return 0x1da8;
            const int count = ScanInt(&ok);
            if (!ok) return 0x1dab;
            out.m_purifyMatItem[1] = item;
            out.m_purifyMatCount[1] = count;
        }
        else if (token == "[invest option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;      // ORIG ctor 0x0822aea6
                if (!ScanStr(&str))
                    break;
                // [b28d-solo-ScriptDepsA] P96 反向：ORIG 用 const_iterator
                //（p167 MISSING const C1ERK/eq/ptE，EXTRA 非 const 版）。
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dbd;
                entry.m_amount = it->second;
                // [b28d-solo-ScriptDepsA] ORIG make_pair 实例为
                // `IRmR23stInvestAmplifyOption_tE`（unsigned long& 非 const）。
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dc3;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1dc7;
                entry.m_value = v;
                out.m_investMap[0].insert(
                    std::make_pair(key, entry));
            }
        }
        else if (token == "[reinvest option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;
                if (!ScanStr(&str))
                    break;
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1dd8;
                entry.m_amount = it->second;
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dde;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1de2;
                entry.m_value = v;
                out.m_investMap[1].insert(std::make_pair(key, entry));
            }
        }
        else if (token == "[random invest upgrade option]")
        {
            while (true)
            {
                stInvestAmplifyOption_t entry;
                if (!ScanStr(&str))
                    break;
                AmplifyTypeMap::const_iterator it = typeMap.find(str);
                if (it == typeMap.end())
                    return 0x1df4;
                entry.m_amount = it->second;
                unsigned long key = static_cast<unsigned long>(ScanInt(&ok));
                if (!ok) return 0x1dfa;
                const int v = ScanInt(&ok);
                if (!ok) return 0x1dfe;
                entry.m_value = v;
                out.m_investMap[2].insert(std::make_pair(key, entry));
            }
        }
        else
        {
        parse_equip_level:
            // 链尾合并默认路径（与 [equip level const] 同一代码）。
            out.m_field3c = ScanInt(&ok);
            if (!ok)
                return 0x1d92;
        }
    }
    return 0;
