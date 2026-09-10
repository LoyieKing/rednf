int AvatarConvert::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_convert_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;               // P164：条件落循环头，复现 4.1 入口直落条件头
        int totalWeight = 0;   // ORIG -0x40：每条记录重置
        // ORIG 0x088b1baf：`new ConvertResult()` 的 value-init 前置零初始化
        // （operator new(0x24) → 8×rep stos dword + 1B → call C1Ev）。
        ConvertResult* convertResult = new ConvertResult();
        convertResult->m_grade = ScanInt((bool*)0);
        int field20 = ScanInt((bool*)0);   // ORIG -0x1c：比较前落槽
        convertResult->m_field20 = (field20 == 1);
        for (;;)
        {
            GetInfo info;
            // P0：ORIG 无具名 key 局部——ScanTypeOrInt 返回值直落 info.m_key。
            info.m_key = ScanTypeOrInt(isType, token, isInt, true);
            if (isType)
                break;   // TYPE → 插入分支
            if (!isInt)
                break;   // EOF/非法 → 丢弃该记录
            info.m_count = static_cast<short>(ScanInt((bool*)0));
            info.m_weight = static_cast<short>(ScanInt((bool*)0));
            convertResult->m_infos.push_back(info);
            totalWeight += info.m_weight;
            convertResult->m_cumWeights.push_back(totalWeight);
        }
        if (isType)
        {
            std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
            if (m_convertMap.find(convertResult->m_grade) == endIt)
            {
                convertResult->m_totalWeight = convertResult->m_cumWeights.back();
                m_convertMap.insert(
                    std::map<int, ConvertResult*>::value_type(convertResult->m_grade,
                                                              convertResult));
            }
            else
            {
                printf("%s duplicate grade:%d", token.c_str(), convertResult->m_grade);
                return -2;
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
