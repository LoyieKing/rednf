// [b28d-38b] 返回类型 bool：ORIG loadItemShopFiles 0x08512b33 调用点
// `xor $0x1,%eax; test %al,%al; je`（P292：无 setcc ⇒ bool；原 int 生 test+sete）。
bool importItemShopScript(ItemShopScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_itemShopScriptBaseDirectory, path))
        return false;
    script->clear();
    // [FragC 试形·P232] ORIG 局部槽基比 4.4 深 4（line -0x18 vs -0x14 等）⇒
    // 补一个 4B 占槽死局部（数组维决定 sub，P232 实证不被 DCE）。无效即回滚。
    char framePad4[4];
    (void)framePad4;
    std::string line;
    std::string line2;
    int value = 0;
    bool ok = false;
    while (ScanType(line, true))
    {
        if (line == "[NPC]")
        {
            script->m_limitCount = ScanInt(static_cast<bool*>(0));
        }
        else if (line == "[message]")
        {
            ScanStr(&script->m_shopName);
        }
        else if (line == "[type]")
        {
            ScanStr(&line2);
            if (line2 == "[etc shop]")
                script->m_limitType = 0;
            else if (line2 == "[weapon shop]")
                script->m_limitType = 1;
            else if (line2 == "[disjoint shop]")
                script->m_limitType = 2;
        }
        else if (line == "[only buy]")
        {
            script->m_field0c = (char)(ScanInt(static_cast<bool*>(0)) != 0);
        }
        else if (line == "[sell item]")
        {
            script->m_curItemList.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_curItemList.push_back(value);
            }
        }
        else if (line == "[sell limit item]")
        {
            script->m_limitNpcBuyList.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_limitNpcBuyList.push_back(value);
            }
        }
        else if ("[tab name]" == line)
        {
            std::string tab;
            while (ScanStr(&tab))
            {
                std::allocator<char> allocator;
                script->m_strList.push_back(
                    std::string(tab.c_str(), allocator));
            }
        }
        else if (line == "[one a day start time]")
        {
            time_t rawTime = time(0);
            struct tm* tmInfo = localtime(&rawTime);
            tmInfo->tm_year = ScanInt(&ok) - 1900;
            if (!ok)
                break;
            tmInfo->tm_mon = ScanInt(&ok) - 1;
            if (!ok)
                break;
            tmInfo->tm_mday = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_hour = ScanInt(&ok);
            if (!ok)
                break;
            tmInfo->tm_min = 0;
            tmInfo->tm_sec = 0;
            script->m_lastUpdateTime = (int)mktime(tmInfo);
        }
        else if (line == "[one a day item]")
        {
            std::vector<int> items;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                items.push_back(value);
            }
            script->m_dailyList.push_back(items);
        }
    }
    script->m_shopId =
        g_itemShopScriptFileList.FindIndexByFullScan(path);
    if (script->m_dailyList.size() != 0)
        script->m_isOneADay = 1;
    return true;
}