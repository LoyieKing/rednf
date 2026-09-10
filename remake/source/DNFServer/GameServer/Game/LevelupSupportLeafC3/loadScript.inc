int LevelupSupportSciprt::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45（ScanTypeOrInt 出参）
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c 当前 [level]
    std::vector<ItemInfo> rewards;   // ORIG -0x58 当前 [step] 奖励暂存

    while (ScanType(token, true))
    {
        if (token == "[step]")
        {
            if (!ScanType(token, true))
                break;
            if (token == "[level]")
                curLevel = ScanInt((bool*)0);
            if (!ScanType(token, true))
                break;
            if (token == "[reward]")
            {
                for (;;)
                {
                    // TYPE 终止奖励序列（该 token 由 ScanTypeOrInt 写入 token 缓冲并消费——忠实）。
                    int itemIndex = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)
                        break;
                    int itemCount = ScanInt((bool*)0);
                    rewards.push_back(ItemInfo(itemIndex, itemCount));
                }
            }
            if (!ScanType(token, true))
                break;
            if (token == "[/step]")
            {
                if (m_stepItems.find(curLevel) != m_stepItems.end())
                {
                    printf("duplicate [level] %d", curLevel);
                    return -1;   // 忠实：重复 [level] 路径不调用 unloadRDARScriptFile
                }
                std::vector<ItemInfo>* levelItems = new std::vector<ItemInfo>();
                levelItems->swap(rewards);   // swap 后 rewards 复位空，供下一 [step] 复用
                m_stepItems.insert(std::make_pair(curLevel, levelItems));
            }
        }
    }

    unloadRDARScriptFile();
    return 0;
