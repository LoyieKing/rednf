bool importRankSystemScript(const char* path, stRankSystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string deadToken;                             // ORIG -0x70 死 string（P171）
    (void)deadToken;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                                     // P164 配套：EOF 落点移到循环外
                                                       // （`return true` 会被 4.4 inline 到检查点后）

        if (token == "[dungeon clear time average]")
        {
            for (;;)
            {
                stRankSystemInfo::stDungeonClearTimeData data;
                // ORIG 0x890ee15..：逐列独立检查，各 `xor $1;test;je` + 共享 break 出口
                data.m_v[0] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[1] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[2] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[3] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[4] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[5] = ScanInt(&ok);
                if (ok != true)
                    break;
                data.m_v[6] = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec38.push_back(data);

                stClearTimeAverageMapKey key;          // ORIG ctor 即 clear
                key.m_a = data.m_v[0];
                key.m_b = data.m_v[1];
                stClearTimeAverage average;
                average.m_v[0] = data.m_v[2];
                average.m_v[1] = data.m_v[3];
                average.m_v[2] = data.m_v[4];
                average.m_v[3] = data.m_v[5];
                average.m_v[4] = data.m_v[6];

                // ORIG：迭代器先默认构造后赋值（P50/P314）+ `==` 比较（P220 反极性）
                std::map<stClearTimeAverageMapKey,
                         stClearTimeAverage>::iterator it;
                it = out->m_dungeonRank.m_map0.find(key);
                // ORIG 0x890efd8..f02c：call eq; test; je <fail 块>，then 直落
                // insert（P222 正向包裹；写 `if (!(it==end))` 会多一条 xor $1）。
                if (it == out->m_dungeonRank.m_map0.end())
                {
                    out->m_dungeonRank.m_map0.insert(
                        std::make_pair(key, average));
                }
                else
                {
                    return false;                      // key 已存在 → 失败
                }
            }
        }
        else if (token == "[dungeon clear time reduction bonus]")
        {
            int a;
            int b;
            // ORIG 0x890f05b..：两列各自 ScanInt+ok!=true break，再 insert/
            // bonus 逐字段/vec18.push_back，尾部 jmp 回循环头。
            for (;;)
            {
                a = ScanInt(&ok);
                if (ok != true)
                    break;
                b = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_map44.insert(std::make_pair(a, b));
                stBonusPointValues bonus;
                bonus.m_a = a;
                bonus.m_b = b;
                out->m_dungeonRank.m_vec18.push_back(bonus);
            }
        }
        else if (token == "[rank grade]")
        {
            int rankIdx = 0;
            int grade;
            // ORIG 0x890f156..：ScanInt → ok!=true break → push_back →
            // rankIdx>8 break → m_rankLevel[rankIdx]=grade → ++rankIdx 回跳。
            for (;;)
            {
                grade = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec5c.push_back(grade);
                if (rankIdx > 8)
                    break;
                out->m_dungeonRank.m_rankLevel[rankIdx] =
                    static_cast<unsigned short>(grade);
                ++rankIdx;
            }
        }
        else if (token == "[combo interval boundaries]")
        {
            for (;;)
            {
                stRankSystemInfo::stComboIntervalBoundary boundary;
                boundary.m_v[0] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[1] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[2] = ScanInt(&ok);
                if (ok != true)
                    break;
                boundary.m_v[3] = ScanInt(&ok);
                if (ok != true)
                    break;
                out->m_vec68.push_back(boundary);
            }
        }
        else if (token == "[combo practice bonus inerval]")
        {
            // ORIG 0x890f293..0x890f2b3：只读一值即回主循环头（0x890f2ad
            // `je +0x4e`），非段内 do-while 循环；ok==false → return 1。
            out->m_field74 = ScanInt(&ok);
            if (!ok)
                break;             // P164 配套：并入循环外共享成功出口
        }
    }
    return true;                                       // ORIG 尾部共享成功出口
