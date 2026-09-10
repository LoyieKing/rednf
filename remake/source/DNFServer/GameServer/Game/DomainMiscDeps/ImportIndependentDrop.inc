int IndependentDropParameterScript::ImportIndependentDrop(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x66;
    }
    std::string token;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 0897caee：跳循环外 return 1
        }
        if (token == "[independent drop]")
        {
            // [b28d-C08 widthscan 判据⑫ clr_l 5 vs 4] ORIG 0x0897c415 在本分支入口清
            // `movl $0x0,-0x44(%ebp)`，其后 0x0897c767（flag==1 `itemId=ScanInt`）与
            // 0x0897c8a9（flag==2 `itemId=m_list04[p].first`）**复用同一槽** ⇒ 原码
            // itemId 是分支级具名 int（非各 while 内声明）；
            // 0x0897c7b9/0x0897c8ec 两处 `lea -0x44(%ebp),%eax` 即引用 pair 构造的
            // 左值实参（P81 `pair<int,int>(int&,int&)`，p167 MISSING
            // `_ZNSt4pairIiiEC2IRiS2_EEOT_OT0_` ×2）。
            int itemId = 0;
            for (;;)
            {
                stIndependentDropInfo info;
                info.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // 段结束（不压栈当前半条）
                }
                info.m_monsterIndex = ScanInt(&ok);
                if (!ok) { return 0x7d; }
                info.m_drop.m_itemIdx = ScanInt(&ok);
                if (!ok) { return 0x81; }
                info.m_drop.m_rate[0] = ScanInt(&ok);
                if (!ok) { return 0x89; }
                info.m_drop.m_rate[1] = ScanInt(&ok);
                if (!ok) { return 0x91; }
                info.m_drop.m_rate[2] = ScanInt(&ok);
                if (!ok) { return 0x99; }
                info.m_drop.m_rate[3] = ScanInt(&ok);
                if (!ok) { return 0xa1; }
                // ORIG 0897c558 单独读 m_rate[4]（错误 0xa5），循环写 m_rate[5+i]
                // （0897c591 `lea 0x4(%ebx),%edx` ⇒ 索引 5..8），再读 m_rate[9]
                // （0897c5ca，错误 0xb0）、m_rate[10]=0（0897c5ec）。旧代码整体错
                // 一格（rate[4..7] 循环 + rate[8] + rate[9]=0）= 真语义 bug。
                info.m_drop.m_rate[4] = ScanInt(&ok);
                if (!ok) { return 0xa5; }
                for (int i = 0; i < 4; ++i)
                {
                    info.m_drop.m_rate[5 + i] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xab;
                    }
                }
                info.m_drop.m_rate[9] = ScanInt(&ok);
                if (!ok)
                {
                    return 0xb0;
                }
                info.m_drop.m_rate[10] = 0;
                info.m_drop.m_pairList.clear();
                info.m_drop.m_rangeMin = ScanInt(&ok);
                if (!ok) { return 0xb7; }
                info.m_drop.m_rangeMax = ScanInt(&ok);
                if (!ok) { return 0xbb; }
                if (info.m_drop.m_rangeMin > info.m_drop.m_rangeMax)
                {
                    return 0xbe;
                }
                info.m_drop.m_levelFilter = ScanInt(&ok);
                if (!ok) { return 0xc3; }
                int flag = ScanInt(&ok);
                if (flag != 0 && flag != 1 && flag != 2)
                {
                    LogManager::logFormat(
                        1, "../RDARScriptIndependentDrop.cpp",
                        __FUNCTION__, 0xca,
                        "flag wrong number(%d)", flag);
                    return 0xcb;
                }
                if (!ok)
                {
                    return 0xcf;
                }

                std::string listToken;
                if (flag == 1)
                {
                    if (ScanType(listToken, true) && listToken == "[list]")
                    {
                        while (true)
                        {
                            itemId = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            int rate = ScanInt(&ok);
                            if (!ok)
                            {
                                return 0xe1;       // ORIG esi=0xe1/ebx=0 → 错误返回
                            }
                            info.m_drop.m_rate[9] += rate;
                            info.m_drop.m_pairList.push_back(
                                std::pair<int, int>(
                                    itemId, info.m_drop.m_rate[9]));
                        }
                    }
                }
                else if (flag == 2)
                {
                    if (ScanType(listToken, true) && listToken == "[list]")
                    {
                        while (true)
                        {
                            int idx = ScanInt(&ok);
                            if (!ok)
                            {
                                break;             // 结束本段明细（ORIG 0897c83a）
                            }
                            bool found = false;
                            for (int e = 0; e < (int)m_vec0.size(); ++e)
                            {
                                if (m_vec0[e].m_field0 != idx)
                                {
                                    continue;
                                }
                                // [b28d-C08 C05④ 第2步试形·已回退] ORIG 0x0897c964
                                // `cmp -0x14(%ebp),%eax; setg %al; test %al,%al; jne` 的
                                // setcc 物化，改 `int p=0; while(p<size())` 后**确实复现**
                                // （OURS setcc 3 = ORIG 3：setle + setg×2，逐条同形同槽），
                                // 但 gap +9→**+10**、difflib equal_ratio 0.828 不变
                                // （本件 519~529 条 >500，N127 台账失真）⇒ **无净收益**
                                // 按纪律回退，维持 for 形（P25 的 while 形钥匙对本件
                                // 只换来 setcc 形态、未换 gap/对齐）。
                                for (int p = 0;
                                     p < (int)m_vec0[e].m_list04.size();
                                     ++p)
                                {
                                    itemId =
                                        m_vec0[e].m_list04[p].first;
                                    int rate =
                                        m_vec0[e].m_list04[p].second;
                                    info.m_drop.m_rate[9] += rate;
                                    info.m_drop.m_pairList.push_back(
                                        std::pair<int, int>(
                                            itemId,
                                            info.m_drop.m_rate[9]));
                                }
                                found = true;
                                break;             // ORIG 命中后跳出线性搜索
                            }
                            (void)found;
                        }
                    }
                }
                m_vecC.push_back(info);
            }
        }
        else if (token == "[dungeon drop rate balance]")
        {
            for (;;)
            {
                stIndependentDropRateControl rc;
                rc.m_itemId = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field04 = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field08 = ScanInt(&ok);
                if (!ok) { break; }
                m_vec18.push_back(rc);
            }
        }
    }
    return 1;
