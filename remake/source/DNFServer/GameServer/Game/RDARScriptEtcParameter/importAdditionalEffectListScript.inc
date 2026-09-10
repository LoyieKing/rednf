int importAdditionalEffectListScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x172c, "%s", path);
        return 0;
    }
    std::string token;
    g_additionalEffectMap_.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;                              // ORIG 0x332 共享出口（P63）
        if (token == "[effect]")
        {
            AdditionalEffectData data;             // ORIG -0x40（每轮外层 body 构造）
            for (;;)
            {
                if (!ScanType(token, true))
                    break;                          // EOF：仅析构 data，**不**写表
            if (token == "[index]")
                data.m_id = ScanInt((bool*)0);
            else if (token == "[type]")
                data.m_mode = ScanInt((bool*)0);
            else if (token == "[draw over]")
                data.m_flag = (unsigned char)(ScanInt((bool*)0) != 0);
            else if (token == "[fix position x]")
                data.m_count = ScanInt((bool*)0);
            else if (token == "[custom ani delay]")
                data.m_min = ScanInt((bool*)0);
            else if (token == "[custom ani delay in town]")
                data.m_max = ScanInt((bool*)0);
            else if (token == "[default term in town]")
                data.m_valueA = ScanInt((bool*)0);
            else if (token == "[default term]")
                data.m_valueB = ScanInt((bool*)0);
            else if (token == "[dash term]")
                data.m_valueC = ScanInt((bool*)0);
            else if (token == "[animation]")
            {
                std::string str;                   // ORIG -0x10 块级一次构造/尾析构
                for (;;)
                {
                    if (!ScanStr(&str))              // ORIG xor$1/test/jne 形
                        break;
                    data.m_strings.push_back(str);
                }
            }
            else if (token == "[/effect]")
            {
                g_additionalEffectMap_[data.m_id] = data;
                break;
            }
            }
        }
    }
    return 1;
