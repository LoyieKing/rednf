int CreateDnfEventSciprt::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_CreateDnf.evt"))   // "tw_CreateDnf.evt" / "Event/"
        return -1;

    std::string token;              // -0x44
    bool isType = false;            // -0x45（ScanTypeOrInt 出参）
    bool isInt = false;             // -0x46
    int curDayLevel = 0;            // -0x4c（[day] 段记录）
    std::vector<ItemInfo> items;    // -0x58

    for (;;)
    {
        goto L_cond;                                    // 入口 jmp（08ac0087）
    L_contA:
        goto L_cond;                                    // continue 中继岛 1（08ac0089: nop;jmp）
    L_contB:
    L_cond:                                             // 条件头（08ac008d）
        if (!ScanType(token, true))
            break;                                      // → break 岛（08ac02be）

        // ORIG 用 operator== + je（无 xor 物化）：必须正向包裹，否定式
        // `if (!(token == X))` 会被 4.4 加 xor（探针实测 ndiff 4→10）。
        if (token == "[step]")                          // 08ac00ab
        {
            if (!ScanType(token, true))                 // 08ac00c2
                break;
            if (token == "[day]")                       // 08ac00e0
                curDayLevel = ScanInt((bool*)0);        // 08ac00f7
            if (!ScanType(token, true))                 // 08ac0106
                break;

            if (token == "[reward]")                    // 08ac0124
            {
                for (;;)                                // reward 体 08ac013b..
                {
                    int rewardIdx = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)                         // 08ac015f：出参为真 → 结束 [reward]
                        break;
                    int count = ScanInt((bool*)0);      // 08ac0167
                    items.push_back(ItemInfo(rewardIdx, count));   // ItemInfo C1 + push_back(move)
                }
            }

            if (!ScanType(token, true))                 // 08ac01a4
                break;
            if (token == "[/step]")                     // 08ac01c2
            {
                // [/step] 收口（08ac01dd..）：查重后经 new 空向量 swap(items) 再以引用拷贝入 map。
                if (m_itemMap.find(curDayLevel) != m_itemMap.end())
                {
                    printf("duplicate [level] %d", curDayLevel);       // 08ac022b
                    return -1;                          // 不调 unloadRDARScriptFile（ORIG 同）
                }
                std::vector<ItemInfo>* staged = new std::vector<ItemInfo>();  // operator new(0xc)
                staged->swap(items);
                m_itemMap.insert(std::pair<const int, std::vector<ItemInfo>*>(curDayLevel, staged));
                // 注【ORIG 原样】：staged 指针直接入 map（value 为 vector*），本体由
                // clearScript 的指针式回收（析构 + delete）承接。
            }
            else
            {
                goto L_contB;
            }
            goto L_cond;                                // 回边（08ac02b9）
        }
        else
        {
            goto L_contA;
        }
    }

    unloadRDARScriptFile();                             // 08ac02c8 收尾路径
    return 0;
