// ============================================================================
// ImportWarAreaCounter —— ORIG T 0x089026f3（init.md:1956，this+0x87b4，
// 固定路径 "Etc/WarArea.etc"）。返回 1=成功 / 0=失败。
// 注：ORIG 无下标越界检查（裸指针写入靠相邻区布局兜底）——照抄裸下标（P171）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82 封顶（原 DIFF 297）ORIG
//   0x089026f3 orig/ours=326/328，P167 CLEAR（32/32）。本波修真语义 bug：
//   ①删三处 ORIG 不存在的越界防御（P140/P171 照抄裸下标）；②[counter] 段复用
//   timeCount 作下标（ORIG 0x08902ae2 把 -0x30 存入 m_num 后清零复用），非独立
//   counter 局部；③doorCount/dungeonIdx 为**块内**局部（ORIG -0x20/-0x18）；
//   ④-0x34 为 ORIG 死局部；⑤maxValue 先于 maxIndex 赋值；⑥make_pair<int&,
//   stWarPoint&>（key 去 const，p167 归零）。残余：全帧槽位 +4 平移（N95/N82）
//   + do-while 尾测 setne 物化（4.4 恒物化，ORIG 直跳 jne）。
// [sweep2-InitScriptProducers_2] 复核：fresh orig/ours=313/316 gap=−3 ndiff=443
//   （恒定 +4 槽平移 + 4 处 setne/je/jne 循环尾物化）；p167 32/32 仅 C1↔C2 同址
//   别名（P93）⇒ 无真缺口，维持 N82 封顶。
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
}