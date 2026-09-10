bool ImportWarAreaCounter(WarAreaCounter* out)  // [P330] 与 DataManagerInitDepends.h 同步
{
    if (!loadRDARScriptFile("", "Etc/WarArea.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xdeb,
                              "CRITICAL ERROR : \xc0\xef\xbf\xaa "
                              "\xc4\xab\xbf\xee\xc5\xcd "
                              "\xbd\xba\xc5\xa9\xb8\xb3\xc6\xae "
                              "\xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return 0;
    }

    std::string token;
    int deadLocal = 0;                     // -0x34：ORIG 初始化后从不读（P171 死局部）
    (void)deadLocal;
    int timeCount = 0;                     // -0x30（[counter] 段复用为下标）
    bool ok = false;                       // -0x69
    int maxValue = 0;                      // -0x2c
    int maxIndex = -1;                     // -0x28

    // P164：纯 goto 循环复位头部条件块（ORIG 0x0890278c），ScanType 失败落
    // 尾部岛（0x08902b54 nop; mov $1,%ebx）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[time table]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x24
                if (!ok)
                    break;
                out->m_timeHourList[timeCount] = v;      // ORIG 裸下标 this+(i+0x18)*4
                ++timeCount;
            }
            goto scan_top;
        }

        if (token == "[door fee]")
        {
            int doorCount = 0;                           // -0x20（块内局部）
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x1c
                if (!ok)
                    break;
                out->m_doorFee[doorCount] = v;           // ORIG 裸下标 this+(i+0x30)*4
                ++doorCount;
            }
            goto scan_top;
        }

        if (token == "[ban interval]")
        {
            out->m_banInterval = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[kick point]")
        {
            out->m_kickPoint = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[ready count]")
        {
            out->m_readyCount = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[wp penalty]")
        {
            out->m_wpPenalty = ScanInt(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[monster exp const]")
        {
            out->m_monsterExpConst = ScanFloat(&ok);
            if (!ok)
                return 0;
            goto scan_top;
        }

        if (token == "[wararea dungeon]")
        {
            int dungeonIdx = 2;                          // -0x18（块内局部，ORIG 从 2 起）
            while (true)
            {
                const int v = ScanInt(&ok);              // -0x14
                if (!ok)
                    break;
                out->m_warAreaDungeon[dungeonIdx] = v;   // ORIG 裸下标
                ++dungeonIdx;
            }
            goto scan_top;
        }

        if (token == "[wp per monster hp]")
        {
            while (true)
            {
                stWarPoint wp;                           // -0x7c，ctor memset 0x10
                // 非 const 左值：ORIG 实例为 make_pair<int&,stWarPoint&>
                // （p167 MISSING `IRiR10stWarPoint`；const 版落 `<IRKiR...>`）。
                int key = ScanInt(&ok);                  // -0x80
                if (!ok)
                    break;                               // 回主循环

                for (int j = 0; j < 4; ++j)              // -0x10
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        return 0;
                    wp.m_pts[j] = v;
                }
                out->m_wpPerMonsterHp.insert(
                    std::make_pair(key, wp));            // greater<int> 键序
            }
            goto scan_top;
        }

        if (token == "[counter]")
        {
            out->m_num = timeCount;                      // +0x140
            timeCount = 0;                               // 复用为 counter（ORIG -0x30）
            do
            {
                const int v = ScanInt(&ok);              // -0xc
                if (!ok)
                    break;
                if (v > maxValue)
                {
                    maxValue = v;
                    maxIndex = timeCount;
                }
                out->m_counterTable[timeCount] = v;      // ORIG 裸下标 this+i*4
                ++timeCount;
            } while (out->m_num != timeCount);
            out->m_curHourIdx = maxIndex;                // ORIG 写 +0xe8
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 1;
