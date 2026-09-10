int importQuestShopScript(const char* path, stQuestShop& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x2815;
    }

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0890fa3d 声明序/槽：
    // token -0x80 / ok -0x81 / text -0x88 / status -0xcc（status 函数头 C1，
    // 每 [quest status] 记录调 clear；旧版块内声明致帧 0xb0 vs ORIG 0xe0）。
    std::string token;                         // ORIG -0x80
    bool ok = false;                           // ORIG -0x81
    std::string text;                          // ORIG -0x88
    stQuestShopStatus status;                  // ORIG -0xcc

    while (ScanType(token, true))
    {
        // ORIG 0x0890fac9：[quest status] 用 operator== 正判、else 落
        // [initialization costs]（p167：MISSING eq / EXTRA ne）；map 插入用
        // make_pair（p167：MISSING make_pair/pair C1<...>OS_ 变体，P61/P192）。
        if (token == "[quest status]")
        {
            // ---- 单个 [quest status] 记录 ----
            status.clear();
            const int idx = ScanInt(&ok);
            if (!ok)
            {
                return 0x2829;
            }
            if (out.m_field18 < idx)
            {
                out.m_field18 = idx;
            }

            bool closeRecord = false;
            while (!closeRecord)
            {
                if (!ScanType(token, true))
                {
                    closeRecord = true;         // ORIG：流结束也先入库再回主循环
                    break;
                }
                if (token == "[/quest status]")
                {
                    closeRecord = true;
                    break;
                }
                if (token == "[name]")
                {
                    ScanStr(&status.m_name);
                }
                else if (token == "[name2]")
                {
                    ScanStr(&status.m_name2);
                }
                else if (token == "[explain]")
                {
                    ScanStr(&status.m_explain);
                }
                else if (token == "[icon]")
                {
                    ScanStr(&status.m_icon);
                    status.m_field10 = ScanInt((bool*)NULL);
                }
                else if (token == "[quest status info]")
                {
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int appendage = loadAppendageType(text);
                    if (appendage == -1)
                    {
                        return 0x284f;
                    }
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int valueType = loadValueType(text);
                    (void)valueType;
                    const float value = ScanFloat((bool*)NULL);
                    (void)value;
                    // ORIG：out+0x28+appendage = (char)idx（appendage 槽位标记）
                    out.m_pad28[appendage] = static_cast<char>(idx);
                }
                else if (token == "[level info]")
                {
                    stStatusLevelInfo info;
                    info.m_second = static_cast<unsigned char>(ScanInt(&ok));
                    if (!ok)
                    {
                        continue;
                    }
                    info.m_first = static_cast<unsigned char>(ScanInt(&ok));
                    if (!ok)
                    {
                        continue;
                    }
                    status.m_levelInfos.push_back(info);
                }
                else if (token == "[quest point]")
                {
                    const int point = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x286d;
                    }
                    status.m_questPoint = static_cast<unsigned char>(point);
                }
            }

            out.m_map0.insert(std::make_pair(idx, status));
        }
        else if (token == "[initialization costs]")
        {
            stQuestShoptInitCost cost;
            cost.m_cost0 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost1 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost2 = ScanInt(&ok);
            if (!ok) { return 0; }
            out.m_initCost.m_cost0 = cost.m_cost0;
            out.m_initCost.m_cost1 = cost.m_cost1;
            out.m_initCost.m_cost2 = cost.m_cost2;
        }
    }
    return 0;
