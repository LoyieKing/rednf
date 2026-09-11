// ============================================================================
// importBloodWarEtcScript —— ORIG T 0x0890e3f8（报告全文转写）
// ============================================================================
int importBloodWarEtcScript(const char* path,
                            stBloodWarGrowTable* growTable,
                            stBloodWarBloodPointTable* bloodPoint,
                            std::vector<stBloodWarBgmTable>* bgm,
                            // [b28d-solo-StScriptImporters] 判词：指令数 338=338、p167 43/43 CLEAR。
                            // [SweepStScriptImporters 2026-09-10 复核] 跳转 62/60：回边 11=11
                            // 方向一致（循环多轮语义完好，无 goto-退出 假匹配），仅少 2 个
                            // 前向 jmp（do-while 入口岛，N 族）。
                            // 本波修复：①register int cont（P231 纯 register，非绑定）复现 ORIG
                            // 全程 %ebx + test %ebx；②if(!ok)/else if(!ScanStr)/else 结构复现
                            // `xor $1;test;je` 物化；③外层与 [BLOOD POINT] 改 for(;;)+break 复现
                            // ORIG 条件头在顶（破 4.4 循环旋转）。
                            // 残留 N：do-while 出口 lowering 极性 + 回边岛位置——ORIG 4.1 发
                            // `test %ebx,%ebx; je <顶部岛 nop;jmp cond>; jmp body`（每分支独立回边岛，
                            // 3 岛在循环头前），4.4 恒 `test %ebx,%ebx; jne body;` 落空合并回边
                            // （岛移至尾部）。for(;;)+break / do-while(true)+break / while(cont!=0)
                            // / goto 尾标签四形实测 348/348/338(setne 劣化)/339，均不可翻 ⇒ N 封顶，
                            // 语义等价（p167 CLEAR）。
                            std::vector<stUltimateWarMonsterInfo>* monsterInfo)
{
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    std::string str;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[BGM]")
        {
            register int cont;    // [b28d-solo-StScriptImporters] ORIG 全程 %ebx + test %ebx（P231 纯 register，非绑定）
            do
            {
                stBloodWarBgmTable bgmTable;
                bgmTable.m_id = ScanInt(&ok);
                if (!ok)
                    cont = false;
                else if (!ScanStr(&str))
                    cont = false;
                else
                {
                    bgmTable.m_name = str;
                    if (bgm)
                        bgm->push_back(bgmTable);
                    cont = true;
                }
            } while (cont);
        }
        else if (token == "[BP TABLE]")
        {
            register int cont;    // [b28d-solo-StScriptImporters] ORIG 全程 %ebx（P231）
            do
            {
                std::vector<std::pair<int, int> > rows;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    rows.push_back(p);
                }
                if (!ok)
                    cont = false;
                else
                {
                    if (growTable)
                        growTable->push_back(rows);
                    cont = true;
                }
            } while (cont);
        }
        else if (token == "[BLOOD POINT]")
        {
            for (;;)
            {
                if (!ScanStr(&str))
                    break;
                int type = 4;
                if (str == "named")
                    type = 0;
                else if (str == "crazy")
                    type = 1;
                else if (str == "round")
                    type = 2;
                else if (str == "nomal")
                    type = 3;
                int v1 = ScanInt(&ok);
                if (!ok)
                    break;
                int v2 = ScanInt(&ok);
                if (!ok)
                    break;
                int v3 = ScanInt(&ok);
                if (!ok)
                    break;
                if (bloodPoint && type != 4)
                {
                    stBloodPoint bp;
                    bp.m_type = type;
                    bp.m_v1 = v1;
                    bp.m_v2 = v2;
                    bp.m_v3 = v3;
                    bloodPoint->push_back(bp);
                }
            }
        }
        else if (token == "[BLOOD INFO]")
        {
            stUltimateWarMonsterInfo info;
            info.m_field0 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field4 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field8 = ScanInt(&ok);
            if (!ok)
                break;
            monsterInfo->push_back(info);
        }
    }
    return 1;
}