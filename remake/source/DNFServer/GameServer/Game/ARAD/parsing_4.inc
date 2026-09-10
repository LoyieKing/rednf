int Arad_OnePlusCardEvent_ItemList::parsing(char* data)
{
    Arad_Script script;
    // [b23判词·AE假阳性] 167/167 归一化逐条相等（含两段 Load 的实参来源、循环体内
    // 默认值先行 store、logFormat 五个 %esp 实参槽序）；残余 diff 全为未 relink .o 的
    // call/jmp 重定位占位与 .rodata/g_dnfLex 数据地址（立即数为节内偏移 + R_386_32，
    // 已逐处核对其字符串内容与 ORIG 0x8bac8xx 一致，§6.3 假阳性），relink 后转
    // IDENTICAL_AE。
    if (!script.Load(data, "item"))
        return 0x1db;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x081a3767/0x081a376d：循环体先写默认值（movw $0x0 到 -0x12、
        // movl $0xffffffff 到 -0x10），再 GetInt 覆盖（P0：具名默认初始化局部）。
        unsigned short webId = 0;
        unsigned long itemIndex = 0xffffffff;
        webId = (unsigned short)script.GetInt(r, "web_id");
        itemIndex = (unsigned long)script.GetInt(r, "item_index");
        if (find_item(webId))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1ea,
                "[ERROR] already exist data id OnePlusCardEvent(id:%d, item:%u)",
                (int)webId, (unsigned int)itemIndex);
            return 0x1eb;
        }
        m_cardList.push_back(Data(webId, itemIndex));
    }
    // ORIG 0x081a3857：第二段 Load 用 g_dnfLex.getInputStreamBuffer() 当前输入缓冲，
    // 非形参 data（item 段 Load 消耗后 lexer 流位置）。
    if (!script.Load(g_dnfLex.getInputStreamBuffer(), "limited_map"))
        return 0x1f3;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        unsigned int map = (unsigned int)script.GetInt(r, "index");
        if (is_limited_map(map))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1fe,
                "[ERROR] already exist limited map index (map:%u)",
                map);
            return 0x1ff;
        }
        m_limitedMap.push_back(map);
    }
    return 0;
