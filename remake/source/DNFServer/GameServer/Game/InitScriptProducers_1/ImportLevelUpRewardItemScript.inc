int ImportLevelUpRewardItemScript(const char* path, stLevelUpRewardItemScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x2aa7;

    std::string token;
    bool ok = false;
    LevelUpRewardItemTableKey key;   // ORIG +0x38 ctor（-0x112，循环外唯一构造）
    LevelUpRewardItemData data;      // ORIG +0x5b ctor（-0x11c）

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[swordman]")
        {
            key.m_job = 0;                       // ORIG +0xa2 movb $0,-0x110
            key.m_grow = ScanInt(&ok);           // ORIG +0xab mov %al,-0x10f
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2ac4;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2ac8;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[fighter]")
        {
            key.m_job = 1;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2add;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2ae1;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[at fighter]")
        {
            key.m_job = 7;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2af6;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2afa;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[gunner]")
        {
            key.m_job = 2;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b0f;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b13;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[at gunner]")
        {
            key.m_job = 5;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b28;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b2c;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[mage]")
        {
            key.m_job = 3;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b41;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b45;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[priest]")
        {
            key.m_job = 4;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b5a;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b5e;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
        else if (token == "[thief]")
        {
            key.m_job = 6;
            key.m_grow = ScanInt(&ok);
            if (!ok)
                break;
            {
                while (true)
                {
                    key.m_lv = ScanInt(&ok);
                    if (!ok) break;
                    data.m_field0 = ScanInt(&ok);
                    if (!ok) return 0x2b73;
                    data.m_field4 = ScanInt(&ok);
                    if (!ok) return 0x2b77;
                    script.m_map0.insert(std::make_pair(key, data));
                }
            }
        }
    }
    return 0;
}
