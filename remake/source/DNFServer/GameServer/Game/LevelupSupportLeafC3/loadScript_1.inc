// ============================================================================
// LevelupSupport2ndScript::loadScript —— 报告 loadScript.md @0x08ab55a4。
// 同上状态机，另含 [job type]（ScanStr → getCharacterJobToEnumValue 写 m_jobType）
// 与 [mail title]/[mail content]（ScanStr 直写两个 string 成员）。重复 [level]
// 路径同样不 unloadRDARScriptFile。
// ============================================================================
// [b28d-N86] fresh 复验 264/267（ORIG 0x08ab55a4）：同 LevelupSupportSciprt::
// loadScript（0x08ab514c）——string/vector/map 局部槽整体平移 + swap 形差异。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
int LevelupSupport2ndScript::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport2nd.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c
    std::vector<ItemInfo> rewards;   // ORIG -0x58

    // [M2S1] 同 LevelupSupportSciprt::loadScript：ORIG 0x08ab5679..0x08ab5683
    // `call; xor $1; test %al; jne`（顶部条件）⇒ `for(;;) + if(!ScanType(...)) break;`。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
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
                // [M2S1] 同 LevelupSupportSciprt::loadScript：ORIG 0x08ab5xxx 单次
                // value_type ctor（IRiRS6_E ↗）→ insert，去掉 make_pair 中间临时。
                m_stepItems.insert(std::map<int, std::vector<ItemInfo>*>::value_type(curLevel, levelItems));
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
}