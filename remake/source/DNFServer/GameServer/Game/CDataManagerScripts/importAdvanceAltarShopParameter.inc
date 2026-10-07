// ============================================================================
// advancealtar::AdvanceAltarShopParameter::importAdvanceAltarShopParameter
// （ORIG 0x088a10cc T，恒返 0）
// 语义（ORIG 汇编逐段还原，见 importAdvanceAltarShopParameter.md ## 汇编/## 反编译 C）：
//  · [star reset fee gold] 段：每轮先 ctor _StarRestFeeByGold(-0x238)，key=ScanInt，
//    fee=ScanInt，任一失败仅结束本段回外层扫描头；成功 insert(key,fee)。
//  · 每个 [ridable id] 块：ridableId=ScanInt；!ok 或 !isValidRidableId ⇒ bVar4=假
//    ⇒ 不插入直接返 0（原实现误无条件插入，已修正）。valid 时 shop.m_nField0=id，
//    进 LAB_088a12ab 段循环，[unit]/[skill]/[tower] 各自内联展开三份独立解析块。
//  · item [id] 子块字段序：unit/tower=[unlock stage index],[name],[field name],[list];
//    skill 额外在 unlock 与 name 间插 [is settable slot]。
//  · 关键：push_back(item) 绑定在 [list] 处理尾（非 [/id]），故 item 收集入
//    shop.m_vecT0/T1/T2 由 [list] 触发；[list] 逐行读入 item.m_listMap(key=FieldType-0
//    行 m_value)，直到某行 ScanStr/ScanInt 失败。[/ridable id] 或 EOF ⇒ 插入 shop。
// [b28c-18-unbind] 32 处 bool-callee 检查点的 register 绑定已全域清除（P0 硬约束），
// 统一落 N96+N18a+P168a④ 判词；xor 物化形为跨版本寄存器分配产物，源码不可复现。
// 语义已全量还原为 ORIG 三段内联真形（our_insns
// 708→1403→1431 vs ORIG 1441，gap 727→38→10）。残留 10 指令差为 gcc 4.1.2 相对
// 4.4.7 的 EH landing-pad 与 BuyShopData/BuyUpgradeData 双份 dtor epilogue vs merge、
// deferred-write 寄存器轮换（技巧24 落点岛/技巧31 dead 撑帧无法从源码复现）
// （范式库「负例 N 类·跨版本：delete[]+成员 dtor EH cleanup / EH 双份 dtor epilogue vs
// merge」），非源码可表达；证据 ORIG 0x088a18de/0x088a2731/0x088a2704 CatchHandler 区段。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（201/201）—— 本轮修 6 处调用点
// 符号差：①`item.m_fieldTypes.push_back(具名 FieldType::T 左值)` ×3（ORIG 调
// `push_back(const FieldType::T&)` RKS2_，右值临时会选 `push_back(FieldType::T&&)` OS2_）；
// ②`!(token == "[id]")` ×3（ORIG 调 operator==，写 != 会发 operator!=）。
// 残余 = N82 超大件帧槽（本轮复核：帧 sub $0x320 vs ORIG $0x350（Δ0x30=12 槽）、
// token -0x208 vs ORIG -0x230（Δ0x28），而 unkC -0x64 与 ORIG 同位 ⇒ 差集中在
// 深区局部区，属 4.1/4.4 聚合/标量槽分区策略差；gap 7、p167 201/201 CLEAR，
// 按 §6.2 三件套封顶）；ORIG 0x088a10cc。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 1441 / ours 1435；p167 201/201
// （差集全为 C1/C2·D1/D2 别名 P93 伪差）；jcc ORIG 84 / 我方 86，上下文差集全部为
// `test;je;jmp`（ORIG 岛形）↔ `test;jne;jmp`（我方）+ 槽平移对齐伪影 ⇒ 维持 N82/P164。
// [GW20Run 2026-09-11 复核] fresh 1441/1434；P196 符号性 3 处 setg 已落（OUR setg×3
// = ORIG，seta 归零）⇒ 余 setcc 直跳形 + N82 帧/槽分区；p167 74=74 CLEAR ⇒ 维持。
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
                                        // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x088a15d4/0x088a1c9c/0x088a2303：
                                        // ScanStr 成功 → operator=(fd.m_str) 并继续；失败 → rowOk=false 跳出。
                                        // 旧写法极性反了（成功即中止整个 [list]，失败才赋值）。
                                        if (ScanStr(&s))
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
                                        // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x088a15d4/0x088a1c9c/0x088a2303：
                                        // ScanStr 成功 → operator=(fd.m_str) 并继续；失败 → rowOk=false 跳出。
                                        // 旧写法极性反了（成功即中止整个 [list]，失败才赋值）。
                                        if (ScanStr(&s))
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
                                        // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x088a15d4/0x088a1c9c/0x088a2303：
                                        // ScanStr 成功 → operator=(fd.m_str) 并继续；失败 → rowOk=false 跳出。
                                        // 旧写法极性反了（成功即中止整个 [list]，失败才赋值）。
                                        if (ScanStr(&s))
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
}