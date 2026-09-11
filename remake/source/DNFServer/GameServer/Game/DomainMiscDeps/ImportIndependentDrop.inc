// ORIG 0x0897c398（0x78f 字节）逐分支转写。返回值=出错行，0=成功：
//   0x66 装载失败；0x7d..0xcf 各 ScanInt/校验失败；0xe1 [list] 段第二列失败；
//   扫描自然结束返回 1。[independent drop] 段每条记录 16 个整数 +
//   flag(0=直接入表,1=[list] 明细,2=[list] 合并既有列表)，累积主掉率写
//   m_drop.m_mainRate，(itemId,累积率) 入 m_drop.m_pairList 后整条入 m_vecC。
//   [dungeon drop rate balance] 段每条 3 整数入 m_vec18。
// b28d-solo-DomainMiscDeps: 真 BUG 已修（m_rate 索引错一格 + 补 1 ScanInt）；
// DIFF 残余（p167 CLEAR 57/57；N82 槽基）
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
            break;                                 // ORIG 0897caee：esi=0 ⇒ 循环外 return 0
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
                // [L3Free25to36 N82 复核·负结论] ORIG 0x0897c583-0x0897c594：
                // `mov -0x24(%ebp),%ebx; lea 0x4(%ebx),%edx; mov %eax,-0x8c(%ebp,%edx,4)`
                // = 基址取 &m_rate[1]（-0x8c）+ 索引 (i+4)；4.4.6 恒 `mov i,%eax;
                // lea 0x5(%eax),%ebx; mov %eax,-0x90(%ebp,%ebx,4)`（基址 m_rate[0] +
                // 索引 i+5）。落点地址两侧全等（-0x7c+4i = m_rate[5+i]），仅常量拆
                // 分点不同；换书写序 `i + 5` 实测零变化 ⇒ 4.1 独有寻址形，封顶。
                for (int i = 0; i < 4; ++i)
                {
                    info.m_drop.m_expect[i] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xab;
                    }
                }
                info.m_drop.m_limit = ScanInt(&ok);
                if (!ok)
                {
                    return 0xb0;
                }
                info.m_drop.m_mainRate = 0;
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
                    if (ScanType(listToken, true))
                    {
                    if (listToken == "[list]")
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
                            // ORIG 0x0897c7a3 累积槽 -0x68 = info+0x34 = **m_rate[10]**
                            // （`movl $0x0,-0x68(%ebp)` 就在 0x0897c5ec 清零同一槽）
                            // ⇒ 累积目标与 pair 第二元同为 m_rate[10]，非 m_rate[9]。
                            info.m_drop.m_mainRate += rate;
                            info.m_drop.m_pairList.push_back(
                                std::pair<int, int>(
                                    itemId, info.m_drop.m_mainRate));
                        }
                    }
                    }
                }
                else if (flag == 2)
                {
                    if (ScanType(listToken, true))
                    {
                    if (listToken == "[list]")
                    {
                        while (true)
                        {
                            // P212/P309：ORIG 0x0897c828 `movb $0x0,-0x19(%ebp)` 在
                            // 0x0897c832 `call ScanInt` **之前**（槽 -0x19 深于 idx 的
                            // -0x18）⇒ found 声明在 idx 之前。
                            bool found = false;
                            int idx = ScanInt(&ok);
                            if (!ok)
                            {
                                break;             // 结束本段明细（ORIG 0897c83a）
                            }
                            for (int e = 0; e < (int)m_vec0.size(); ++e)
                            {
                                // ORIG 0x0897c86c `cmp -0x18(%ebp),%eax; sete %al` = 正极性
                                // `==`；搜索体后 0x0897c949 `cmpb $0x0,-0x19(%ebp);
                                // je <++e>` = **体尾独立 `if (found) break;`**（非命中点
                                // 内 break）⇒ 原码 `if (==) {...; found = true;} if (found) break;`。
                                if (m_vec0[e].m_field0 == idx)
                                {
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
                                    info.m_drop.m_mainRate += rate;
                                    info.m_drop.m_pairList.push_back(
                                        std::pair<int, int>(
                                            itemId,
                                            info.m_drop.m_mainRate));
                                }
                                found = true;
                                }
                                if (found)
                                {
                                    break;             // ORIG 命中后跳出线性搜索
                                }
                            }
                            (void)found;
                        }
                    }
                    }
                }
                m_vecC.push_back(info);
            }
        }
        // ORIG 0x0897c9fa（dead 分支 ebx==0）落点 = 0x0897ca0a（"[dungeon drop rate
        // balance]" 比较）⇒ 段循环 break 后**仍走第二个比较** ⇒ 原码为两个**并列 if**
        // 而非 else-if（else-if 时 4.4.6 把 dead 落点折成循环头，多一对 jmp+nop 岛）。
        if (token == "[dungeon drop rate balance]")
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
    // ORIG 0x0897caef `mov $0x0,%esi; jmp 0x0897cb11`（循环耗尽路径）⇒ 成功返回 **0**
    // （CDataManager.h:2580 注释「0=成功」；调用点 CDataManager.cpp:2718 以 `!= 0` 判错）。
    return 0;
}