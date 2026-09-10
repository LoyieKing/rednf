int LevelupSupport2ndScript::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport2nd.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c
    std::vector<ItemInfo> rewards;   // ORIG -0x58

    while (ScanType(token, true))
    {
        if (token == "[job type]")
        {
            std::string jobName;   // ORIG -0x5c 作用域临时
            ScanStr(&jobName);
            m_jobType = getCharacterJobToEnumValue(jobName);
        }
        else if (token == "[step]")
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
                    return -1;   // 忠实：不 unloadRDARScriptFile
                }
                std::vector<ItemInfo>* levelItems = new std::vector<ItemInfo>();
                levelItems->swap(rewards);
                m_stepItems.insert(std::make_pair(curLevel, levelItems));
            }
        }
        else if (token == "[mail title]")
        {
            ScanStr(&m_mailTitle);      // this+0x1c
        }
        else if (token == "[mail content]")
        {
            ScanStr(&m_mailContent);    // this+0x20
        }
    }

    unloadRDARScriptFile();
    return 0;
