int EmblemCompound::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "emblem_compound_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;
    // ORIG +0x40：`jmp cond` + 条件块在循环体之前（4.1 while 布局）⇒
    // for(;;)+头部 break 复现（P283/P164）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[emblem compound cost]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                int value = ScanInt((bool*)0);   // ORIG +0xb6：value 先扫再查重
                std::map<int, int>::iterator endIt = m_cost.end();
                if (m_cost.find(grade) == endIt)
                    m_cost.insert(std::map<int, int>::value_type(grade, value));
                else
                {
                    printf("[emblem compound cost] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem compound result]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x1c2：new Result()（P90 value-init 前置清零循环 2×movw+movb，
                // 覆盖 +0..+8，跳过 +9 对齐 pad）后逐字段写入。
                Result* result = new Result();
                result->m_listIdx = static_cast<short>(ScanInt((bool*)0));
                result->m_itemIndex = static_cast<short>(ScanInt((bool*)0));
                result->m_itemCount = static_cast<short>(ScanInt((bool*)0));
                result->m_field6 = static_cast<short>(ScanInt((bool*)0));
                result->m_selectable = (ScanInt((bool*)0) == 1);
                std::map<int, Result*>::iterator endIt = m_result.end();
                if (m_result.find(grade) == endIt)
                    m_result.insert(std::map<int, Result*>::value_type(grade, result));
                else
                {
                    ::operator delete(result);
                    printf("[emblem compound result] duplicate grade:%d", grade);
                    return -2;
                }
            }
        }
        else if (token == "[emblem list]")
        {
            for (;;)
            {
                int listIdx = ScanTypeOrInt(isType, token, isInt, true);
                if (isType || !isInt)
                    break;
                // ORIG +0x36c：ScanInt 结果先落 int 槽(-0xc)，再转 unsigned long
                // 转换临时槽(-0x18)；p167 实证 ORIG 调 push_back(T&&)（EOm）⇒
                // 实参必须是 prvalue（禁具名左值，否则选 RKm 左值重载）。
                int itemIdx = ScanInt((bool*)0);
                // ORIG 直接 this + listIdx*12 + 0x30 索引（无边界检查，忠实保留）。
                m_list[listIdx].push_back(static_cast<unsigned long>(itemIdx));
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
