int importQPGuideScript(const char* path, stQPGuideScript& out)
{
    // [SweepStScriptImporters 2026-09-10] 判词：AE（本件 DIFF→AE；TU 四数 227/448/0/3）。
    // 对照 ORIG 0x0890ff93 逐块修复（本波，原 388/396）：
    //  ① 推荐段 ScanStr(&jobStr) 成功路径补 `bRec2 = true;`（ORIG 0x08910092 movb $1,-0x16）；
    //  ② ScanStr(&growStr) 改否定形 if(!...)（复现 ORIG 0x089100ea xor$1;test;je <成功>）；
    //  ③ data.m_job=job 前置于 int grow=-1（ORIG 0x089100ca/0x089100cd）；
    //  ④ data.m_grow=grow 前置于 int count=0（ORIG 0x0891012e/0x08910134）；
    //  ⑤ tooltip 段 key/val 声明移出 while 体 + 体内 key=-1 重置（ORIG 0x089102ba/c1/d1）；
    //  ⑥ tooltip empty 检查改否定 if/else（ORIG 0x089102f8 xor$1;test;je <return0>）；
    //  ⑦ val.compare("") 改「空 then + goto 尾标签」形（P223，复现 ORIG 0x08910367
    //     sete;test;jne <nop 岛 0x0891043c>）；
    //  ⑧ map find 改具名左操作数 iterator it=find(key); if(it==end())（P219：翻求值序
    //     end→find 为 ORIG 的 find→end，sret 槽 -0x6c/-0x38 与 ORIG 全等）；
    //  ⑨ 尾部 3×if-return 改单一 return(!bRec&&!bRec2&&!bTool)（ORIG 0x0891047f 三连
    //     movzbl;xor$1;test;je + movzbl %al,%ebx）。
    // 验证：insns 396=396；**jmp+jcc 67/67 目标下标逐位全等**（含无条件 jmp，mismatch=0；
    // 回边 4=4 方向一致 ⇒ 非 clean() 目标归一假匹配，无 goto-退出）；结构化 diff 仅剩
    // 4 处字符串立即数（ORIG 报告 $"..."/我方 .o 重定位占位，AE 归一化）；p167 48/48
    // （MISSING/EXTRA 仅 C1/C2、D1/D2 同址别名，P93）；槽位集全等（token -0x3c / data -0x50 /
    // jobStr -0x54 / growStr -0x58 / count -0x5c / itemIdx -0x60 / key -0x64 / val -0x68 /
    // find -0x6c / end -0x38 / job -0x14 / grow -0x10 / i -0xc / bRec2 -0x16 / bRec -0x17）。
    if (!loadRDARScriptFile("", path))
        return 0x28b6;
    std::string token;
    bool bRec = true;    // [qp recommendation] 段可解析标志（-0x17）
    bool bRec2 = true;   // [qp recommendation] 记录循环标志（-0x16）
    bool bTool = true;   // [qp tooltip mapping] 循环标志（-0x15）
    stQPGuideScript::stRecommendQPData data;

    while (bRec)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[qp recommendation]")
        {
                while (bRec2)
                {
                    data.clear();
                    int job = -1;
                    std::string jobStr;
                    if (!ScanStr(&jobStr))
                    {
                        // ScanStr 失败：out[10] 已填充则结束记录段，否则返回 0。
                        if (!out.m_vec0.at(10).empty())
                        {
                            bRec2 = false;
                            break;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        bRec2 = true;
                        job = getCharacterJob(jobStr.c_str());
                        if (job < 0 || job > 10)
                            return 0;
                        data.m_job = job;
                        int grow = -1;
                        std::string growStr;
                        if (!ScanStr(&growStr))
                        {
                            return 0;
                        }
                        else
                        {
                            grow = getGrowTypeIndex(growStr.c_str());
                            if (grow == 0xffff)
                                return 0;
                            data.m_grow = grow;
                            int count = 0;
                            if (!ScanInt(&count))
                                return 0;
                            if (count <= 0)
                                return 0;
                            int itemIdx = -1;
                            for (int i = 0; i < count; ++i)
                            {
                                if (!ScanInt(&itemIdx))
                                    return 0;
                                data.m_items.push_back(itemIdx);
                            }
                            if (job < 0 || job > 10)
                                return 0;
                            if (out.m_vec0.size() != 0xb)
                                return 0;
                            out.m_vec0[job].push_back(data);
                        }
                    }
                }
            }
            else if (token == "[qp tooltip mapping]")
            {
                int key = -1;
                std::string val;
                while (bTool)
                {
                    key = -1;
                    if (!ScanInt(&key))
                    {
                        if (!out.m_tooltipMapC.empty())
                        {
                            bTool = false;
                            bRec = false;
                            break;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    if (key < 0)
                        return 0;
                    if (!ScanStr(&val))
                        return 0;
                    if (!val.compare(""))
                        goto tooltip_skip;
                    {
                        std::map<int, std::string>::iterator it = out.m_tooltipMapC.find(key);
                        if (it == out.m_tooltipMapC.end())
                        {
                            out.m_tooltipMapC.insert(std::make_pair(key, val.c_str()));
                        }
                    }
tooltip_skip:;    // [P223] 空 then 占位岛锚点（复现 ORIG 0x0891043c 的 nop 岛）
                }
            }
    }
    return (!bRec && !bRec2 && !bTool);
