bool importUnlimitChallengeScript(const char* path, stUnlimitChallengeInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;     // ORIG -0xfc（先构造）
    std::string sub;       // ORIG -0x100
    int zeroInit = 0;      // ORIG -0x34：movl $0x0 后不再读（帧槽对齐保留）
    bool ok = false;       // ORIG -0x101
    (void)zeroInit;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                               // EOF → 成功

        if (token == "[onoff]")
        {
            int value = ScanInt(&ok);                  // -0x30
            if (!ok)
                break;                                 // P164 配套：并入共享成功出口（0x0890bd1a）
            if (value == 0)
                out->m_field0 = false;                 // movb $0,(%eax)
            else
                out->m_field0 = true;                  // movb $1,(%eax)
        }
        else if (token == "[server_id]")
        {
            int value;                                 // -0x108
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec4.push_back(value);
            }
        }
        else if (token == "[level]")
        {
            int value;                                 // -0x10c
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec10.push_back(value);
            }
        }
        else if (token == "[reward_server]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x2c（首列=等级）
                int link;                              // -0x28（链上死槽）
                int col2;                              // -0x24（第二列）
                int col3;                              // -0x20（第三列）
                // ORIG 0x0890be2e：movl $0,-0x20 → -0x24 → -0x28 → -0x2c 链式
                // 置零（4.1.2 先声明近 ebp；4.4.7 相反，故声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;                     // 段结束 → ScanType 下一子标签
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x118
                        info.m_jobLevel = col1;
                        info.m_subIdx = 0;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x124
                        info.m_jobLevel = col1;
                        info.m_subIdx = 1;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x130
                        info.m_jobLevel = col1;
                        info.m_subIdx = 2;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x13c
                        info.m_jobLevel = col1;
                        info.m_subIdx = 3;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x148
                        info.m_jobLevel = col1;
                        info.m_subIdx = 4;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x154
                        info.m_jobLevel = col1;
                        info.m_subIdx = 5;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x160
                        info.m_jobLevel = col1;
                        info.m_subIdx = 6;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
            }
        }
        else if (token == "[reward_client]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x1c（首列=等级）
                int link;                              // -0x18（链上死槽）
                int col2;                              // -0x14（第二列）
                int col3;                              // -0x10（第三列）
                // ORIG 0x0890c45d 链式置零（同 reward_server，声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x164
                        stUnlimitChallengeItem item;   // -0x16c
                        key.m_a = col1;
                        key.m_b = 0;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x170
                        stUnlimitChallengeItem item;   // -0x178
                        key.m_a = col1;
                        key.m_b = 1;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x17c
                        stUnlimitChallengeItem item;   // -0x184
                        key.m_a = col1;
                        key.m_b = 2;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x188
                        stUnlimitChallengeItem item;   // -0x190
                        key.m_a = col1;
                        key.m_b = 3;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x194
                        stUnlimitChallengeItem item;   // -0x19c
                        key.m_a = col1;
                        key.m_b = 4;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1a0
                        stUnlimitChallengeItem item;   // -0x1a8
                        key.m_a = col1;
                        key.m_b = 5;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1ac
                        stUnlimitChallengeItem item;   // -0x1b4
                        key.m_a = col1;
                        key.m_b = 6;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
            }
        }
        else if (token == "[message]")
        {
            for (;;)
            {
                int id = 0;                            // -0xc
                std::string msg1;                      // -0x1b8
                std::string msg2;                      // -0x1bc
                id = ScanInt(&ok);
                if (!ok)
                    break;                             // 0x0890cd6f test ebx → 退出段
                ScanStr(&msg1);
                ScanStr(&msg2);
                stUnlimitChallengeMessageInfo info;   // -0x1c8（C1 0x891c84a）
                info.m_id = static_cast<short>(id);
                info.m_msg1 = msg1;
                info.m_msg2 = msg2;
                out->m_vec28.push_back(info);
            }
        }
    }
    return true;
