bool importGrowthPower(char* path, STGrowthPowerData* out)
{
    if (out == 0)
        return false;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string group;

    // P164：4.4 把 while 旋转成尾部条件头；纯 goto 循环复位到 ORIG 头部条件块
    // （0x08911f38），ScanType 失败落尾部岛（0x08912244 nop; mov $1,%ebx）。
    // 每段循环内各有一个 bool ok 局部（ORIG 槽 -0x19/-0x1d 独立，且循环头预清）。
    // [package selection] 的内层扫描是 `bool ScanInt(int*)`（ORIG 0x08912136
    // `_Z7ScanIntPi`，返回值即成功标志），不是 `int ScanInt(bool*)`。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[breakaway section]")
        {
            while (true)
            {
                bool ok = false;
                STBreakAwaySection section;
                section.m_field0 = (char)ScanInt(&ok);
                if (!ok)
                    break;
                section.m_field1 = (char)ScanInt((bool*)0);
                section.m_field2 = (char)ScanInt((bool*)0);
                out->m_vec0.push_back(section);
            }
            goto scan_top;
        }

        if (token == "[exp reward]")
        {
            while (true)
            {
                bool ok = false;
                STExpReward reward;                      // ctor 仅清前 4 字节（同 ORIG）
                // ORIG 逐字节写入 +0x00..+0x03（mov %al）：
                //   +0x00=m_reward 低字节、+0x01=m_field4、+0x02=m_field5、
                //   +0x03=m_powerUp。
                reward.m_reward8 = (char)ScanInt(&ok);
                if (!ok)
                    break;
                reward.m_field4 = (char)ScanInt((bool*)0);
                reward.m_field5 = (char)ScanInt((bool*)0);
                reward.m_powerUp = (char)ScanInt((bool*)0);
                out->m_vecC.push_back(reward);
            }
            goto scan_top;
        }

        if (token == "[package reward]")
        {
            do
            {
                if (!ScanType(token, true))
                    goto scan_top;                       // EOF → 回主循环

                if (token == "[group]")
                {
                    if (!ScanStr(&group))
                        return false;

                    const int job = getCharacterJob(group.c_str());
                    const int n = ScanInt((bool*)0);
                    const int key = makeFairPVPEquipmentPackageKey(job, n);

                    std::vector<std::vector<STStackablePackageData> > groups;
                    ScanType(token, true);
                    while (token == "[package selection]")
                    {
                        int idx;
                        std::vector<STStackablePackageData> items;
                        while (true)
                        {
                            if (!ScanInt(&idx))
                                break;
                            STStackablePackageData d;
                            d.m_field0 = idx;
                            d.m_field4 = ScanInt((bool*)0);
                            items.push_back(d);
                        }
                        groups.push_back(items);
                        ScanType(token, true);
                    }
                    out->m_map18[key] = groups;
                }
                // ORIG 0x08912232 只有 operator==（无 operator!=）：`if (token ==
                // "[/package reward]") break;` 生 `test; je <循环头>` + fall 跳出。
                if (token == "[/package reward]")
                    break;
            } while (true);
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return true;
