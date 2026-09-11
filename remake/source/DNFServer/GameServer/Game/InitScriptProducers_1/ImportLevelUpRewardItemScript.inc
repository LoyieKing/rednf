// ORIG T 0x089107e9。八个职业分支各自携带独立内联解析循环与临时对象槽（ORIG
// 汇编中 make_pair/pair/insert 临时槽按 swordman..thief 分支序自 -0xf4 依次
// 分配至 -0x14，即每分支一条独立 insert(std::make_pair(key,data)) 语句）；
// key/data 对象构造在 ScanType 主循环前（ORIG +0x35..+0x5b）。未知 token 跳过。
// 收敛口径：orig=ours=607，助记符序列全等；仅 8 个 token 串/空串字面量在 .o
// 阶段以未解析重定位显示（.rodata 实测内容逐条一致），链接后按 AE 口径判等。
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
