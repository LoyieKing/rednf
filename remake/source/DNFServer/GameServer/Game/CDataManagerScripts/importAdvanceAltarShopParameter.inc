int advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter(
    const char* path)
{
    // ORIG 0x088a10cc：三种段 [unit]/[skill]/[tower] 各自内联展开（各 ~190 指令），
    // [list] 处理结尾 push_back 当前 item（非 [/id] 绑定），且仅当 isValidRidableId
    // 通过并入 m_buyShopMap 后外层 do-while(bVar4) 才继续，否则返 0。下面按 ORIG
    // 汇编 LAB 布局逐段还原为三块独立代码。
    std::string token;
    bool ok = false;
    // [X2/X1 形·Main 批准配方] ORIG 全部 bool callee 检查为 `xor $0x1,%eax;
    // test %al,%al; j…`（0x088a1101/0x088a112c/0x088a117c/… 共 24 处），4.4 直写
    // !call/!ok 折成 test 单段缺 xor。[b28c-18-unbind] 绑定已全域清除
    // （N96+N18a+P168a④，判词见 DeleteInvalidItemScript::loadScript 头）。
    if (!(loadRDARScriptFile("", path)))
        return 0;

    for (;;)
    {
        // 外层跳过：处理 [star reset fee gold]，直到 [ridable id]（isRidable）或 EOF。
        bool isRidable = false;
        for (;;)
        {
            if (!(ScanType(token, true)))
                break;                          // ORIG 0x088a1136 jne → EOF 返 0
            if (token == "[star reset fee gold]")
            {
                for (;;)
                {
                    _StarRestFeeByGold fee;                 // ORIG 每轮 ctor(-0x238)
                    int key = ScanInt(&ok);                 // ORIG -0x23c
                    if (!(ok))
                        break;                              // 行首失败：结束本段回外层扫描头
                    int feeVal = ScanInt(&ok);              // ORIG -0x60
                    if (!(ok))
                        break;
                    fee.m_field0 = feeVal;                  // ORIG 校验后才 -0x60→-0x238
                    m_starRestMap.insert(std::make_pair(key, fee));
                }
                continue;
            }
            if (token == "[ridable id]")
            {
                isRidable = true;
                break;
            }
        }
        if (!isRidable)
            break;                                          // 文件耗尽 → 返 0

        BuyShopData shop;
        int ridableId = ScanInt(&ok);
        if (!(ok))
            break;                                          // ORIG !ok：bVar4=假 → 不插入返回
        if (!isValidRidableId(static_cast<advancealtar::RidableId::T>(ridableId)))
            break;                                          // ORIG 非法：bVar4=假 → 不插入返回
        shop.m_nField0 = ridableId;

        // LAB_088a12ab 段循环。[/ridable id] 或 EOF 跳出 → 插入 shop。
        for (;;)
        {
            if (!(ScanType(token, true)))
                break;                                      // EOF → 插入
            if (token == "[unit]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;                              // ORIG fail → 回段循环
                    if (token == "[/unit]")
                        break;
                    // p167：ORIG 调 operator==（`_ZSteq...`），写 `!(==)` 保持同一符号。
                    if (!(token == "[id]"))
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;                           // id 扫描失败 → 回段循环
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                {
                                    // p167：ORIG 调 push_back(const FieldType::T&)
                                    // （RKS2_）⇒ 传具名左值（右值临时选 && 重载）。
                                    FieldType::T ftype =
                                        static_cast<FieldType::T>(ft);
                                    item.m_fieldTypes.push_back(ftype);
                                }
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                // [A16 round5 · P196/setcc 符号性] ORIG 0x088a156f 段为
                                // `call size-getter; cmp -0x48(%ebp),%eax; setg %al`
                                // = **有符号**比较（4.1 源码为 int 下标 + (int) 界）；
                                // 我方 size_t 下标使比较提升为无符号 ⇒ `seta`（3 处）。
                                for (int fi = 0;
                                     fi < (int)item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT0.push_back(item);   // ORIG push_back 绑定 [list] 尾
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[skill]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;
                    if (token == "[/skill]")
                        break;
                    if (!(token == "[id]"))
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[is settable slot]")
                        {
                            int v = ScanInt(&ok);
                            if (!(ok))
                                break;
                            item.m_field8 = (v != 0);
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                {
                                    FieldType::T ftype =
                                        static_cast<FieldType::T>(ft);
                                    item.m_fieldTypes.push_back(ftype);
                                }
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                // [A16 round5 · P196/setcc 符号性] 同首处：ORIG
                                // `cmp -0x18(%ebp),%eax; setg`（有符号）⇒ int 下标
                                // + (int) 界，复现 3 处 setg。
                                for (int fi = 0;
                                     fi < (int)item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT1.push_back(item);
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[tower]")
            {
                for (;;)
                {
                    if (!(ScanType(token, true)))
                        break;
                    if (token == "[/tower]")
                        break;
                    if (!(token == "[id]"))
                        continue;
                    BuyUpgradeData item;
                    item.m_nItemId = ScanInt(&ok);
                    if (!(ok))
                        continue;
                    for (;;)
                    {
                        if (!(ScanType(token, true)))
                            break;
                        if (token == "[unlock stage index]")
                        {
                            item.m_field4 = ScanInt(&ok);
                            if (!(ok))
                                break;
                            continue;
                        }
                        if (token == "[name]")
                        {
                            if (!(ScanStr(&item.m_strDesc)))
                                break;
                            continue;
                        }
                        if (token == "[field name]")
                        {
                            std::string fieldName;
                            for (;;)
                            {
                                if (!(ScanStr(&fieldName)))
                                    break;
                                int ft = getFieldType(fieldName);
                                if (ft != -1)
                                {
                                    FieldType::T ftype =
                                        static_cast<FieldType::T>(ft);
                                    item.m_fieldTypes.push_back(ftype);
                                }
                            }
                            continue;
                        }
                        if (token == "[list]")
                        {
                            FieldData fd;
                            FieldDataMap rowMap;
                            bool rowOk = true;
                            while (rowOk)
                            {
                                rowMap.clear();
                                // [A16 round5 · P196/setcc 符号性] 同前两处：ORIG
                                // `cmp -0xc(%ebp),%eax; setg`（有符号）。
                                for (int fi = 0;
                                     fi < (int)item.m_fieldTypes.size(); ++fi)
                                {
                                    FieldType::T ft = item.m_fieldTypes[fi];
                                    fd.m_field0 = ft;
                                    if (getdataType(ft) == 0)
                                    {
                                        std::string s;
                                        if (!ScanStr(&s))
                                                fd.m_str = s;
                                        else
                                            rowOk = false;
                                        if (!rowOk)
                                            break;
                                    }
                                    else
                                    {
                                        fd.m_value = ScanInt(&ok);
                                        if (!(ok))
                                        {
                                            rowOk = false;
                                            break;
                                        }
                                    }
                                    if (rowOk)
                                        rowMap.insert(std::make_pair(ft, fd));
                                }
                                if (rowOk)
                                {
                                    int key = rowMap[FieldType::FT_LEVEL].m_value;
                                    item.m_listMap.insert(
                                        std::make_pair(key, rowMap));
                                }
                            }
                            shop.m_vecT2.push_back(item);
                            continue;
                        }
                        if (token == "[/id]")
                            break;
                    }
                }
                continue;
            }
            if (token == "[/ridable id]")
                break;
        }
        // ORIG LAB_088a267d：valid 路径下 [/ridable id] 或 EOF 都把 shop 入 map。
        m_buyShopMap.insert(std::make_pair(shop.m_nField0, shop));
    }
    return 0;
