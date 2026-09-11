// 按 docs/class_func_reports/_lt_global_gt_/importSecretShopScript.md 忠实转写：
//   - 所有取值扫描均为 int ScanInt(bool* ok) 形式；占位扫描传 (bool*)0
//     （ORIG movl $0x0 压参），成功标志仅用于条目循环终止判断。
//   - [dungeon npc]/[level npc]：每条 7 次扫描（key/npcIdx/rate 带 flag +
//     4 次 flag=0 占位，ORIG +0x11c 循环 i=0..3），循环至 flag 失败后
//     verifyNpc 对应 multimap。
//   - [npc]：npcKey 存入局部 stNpcSales::m_field0；内层 token 循环处理
//     [dungeon]/[level]（先 dungeonKey/maxCount 两次占位扫描，随后 itemIdx
//     带 flag 的商品循环）、[/npc]（整表插入 m_dungeonNpcMap）；
//     [dungeon cash item] 为无操作继续。
//   - [event item] 在外层：首个占位扫描写 m_eventDungeonSales.m_maxCount
//     （ORIG script+0x64），其后 itemIdx 带 flag 的商品循环插入 m_sales。
//   - stDungeonNpc 各字段与 key/npcIdx/rate 的对应为 [推断]（ORIG 经局部
//     辅助构造函数写入，未逐指令核对）。
int importSecretShopScript(SecretShopScript* script, const char* path)
{
    // [b28d-solo-CSecretShop 判词·N + 真 bug 修复] 超大件按 §6.2 三件套：
    // ① p167 调用点差集 CLEAR（106=106，仅 C1/C2、D1/D2 同址别名伪差，P93）；
    // ② gap = -2（ORIG 644 vs 我们 646）；③ 残差为控制流岛布局（4.1 循环头
    //    `jmp/nop/jmp/nop` 双岛 + continue 岛 vs 4.4 单 jmp）与 N82 槽号差。
    // 本轮修复（ORIG 证据）：① 分支物理序改 ORIG 序 [dungeon npc]→[level
    //    section]→[level npc]（P324，ndiff 113→45 行）；② [dungeon npc]/[level
    //    npc] 的 ScanInt 结果直接写 stDungeonNpc 字段（ORIG -0x260 对象槽、pair
    //    两实参同址），删 3 条拷贝；③ 补 pScript 具名局部（ORIG -0x18，P0）；
    //    ④ 补死局部 unusedSlot（ORIG -0x14 死初始化，P232）；⑤ **[真 bug]**
    //    [secret shop end] 置 flag=true 且函数返回该 flag（ORIG 0x08a827b8
    //    movb $1 + 尾 0x08a827c2 movzbl 返回；旧实现恒 return 1）；⑥ [add slot
    //    cash user stackable count] 补 continue（ORIG 0x08a82794 jne 岛，P223）。
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    // [b28d-solo-CSecretShop] ORIG 0x08a81d2b `mov 0x8(%ebp),%eax; mov
    // %eax,-0x18(%ebp)`，其后全程经 -0x18 读 script（14 处）⇒ 原码有具名
    // 指针局部（P0 槽往返律），补之；旧实现直读形参少 2 条 store/reload。
    SecretShopScript* pScript = script;
    pScript->clear();
    std::string token;
    // [b28d-solo-CSecretShop] ORIG 0x08a81d3e `movl $0x0,-0x14(%ebp)`：全函数
    // 无读的死初始化槽（P232：未使用局部照样占帧/发射初始化），补之以对齐槽图。
    int unusedSlot = 0;
    (void)unusedSlot;
    // [b28d-solo-CSecretShop] p167：ORIG 的 [dungeon npc]/[level npc] 是两段
    // 独立代码（[level npc] 为 1 带 flag + 2 无 flag + 4 占位），旧实现合并导致
    // ScanInt 调用点少 12；flag 为函数级单 bool（ORIG local_255）。
    bool flag = false;
    // [F-P168a 试形] ORIG while 头 `call ScanType; xor $0x1,%eax; test %al,%al;
    // jne exit`（0x08a81d5a 段）= bool callee 裸取反形 ⇒ 源形为
    // `if (!ScanType(token, true)) break;`（同 BlueMarbleTileScript/importScript.inc
    // 先例）；`while (ScanType(...))` 4.4 直产 `test %al; jne` 少 xor。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[dungeon npc]")
        {
            for (;;)
            {
                // [b28d-solo-CSecretShop] ORIG 0x08a81e0x：ScanInt 结果**直接**写
                // stDungeonNpc 字段（对象槽 -0x260，pair 两实参同址 &npc）——旧
                // 实现用独立 key/npcIdx/rate 再拷贝，多 3 条 mov 且帧 0x330 vs
                // ORIG 0x320。
                stDungeonNpc npc;
                npc.m_field0 = ScanInt(&flag);   // [推断]
                if (!flag)
                    break;
                npc.m_npcIdx = ScanInt(&flag);
                if (!flag)
                    break;
                npc.m_rate = ScanInt(&flag);
                if (!flag)
                    break;
                for (int i = 0; i < 4; ++i)
                    ScanInt((bool*)0);   // 4 个占位扫描（ORIG +0x11c）
                pScript->m_npcMap.insert(
                    std::pair<int, stDungeonNpc>(npc.m_field0, npc));
            }
            pScript->verifyNpc(pScript->m_npcMap);
        }
        // [b28d-solo-CSecretShop] ORIG 分支物理序（按串地址）：[dungeon npc]
        // 0x08a81d73 → [level section] 0x08a81ea6 → [level npc] 0x08a81f5e
        // ⇒ if-else 链必须同序（P324 叶块序=书写序）。
        else if (token == "[level section]")
        {
            for (;;)
            {
                // [M5D_SecretShop] ORIG 0x08a81ecd/fef/f0b：三次 ScanInt 分别落
                // **三个独立标量槽** -0x26c/-0x268/-0x264，随后 0x08a81f10-
                // 0x08a81f2e 三组 `mov -0xNNN,%eax; mov %eax,-0x22N` 才拷入
                // set::insert 实参对象 -0x224..-0x218（ORIG 独有段，旧形直写
                // 字段导致我方此处少 6 条）⇒ 原码为「3 个具名 int + 逐字段赋值」。
                int levelIdx = ScanInt(&flag);
                if (!flag)
                    break;
                int minLev = ScanInt((bool*)0);
                int maxLev = ScanInt((bool*)0);
                stLevelSection section;
                section.m_levelIdx = levelIdx;
                section.m_minLev = minLev;
                section.m_maxLev = maxLev;
                pScript->m_levelSections.insert(section);
            }
        }
        else if (token == "[level npc]")
        {
            for (;;)
            {
                // [M5D_SecretShop] ORIG 0x08a81f8a/fae/fc0：三次 ScanInt 结果落
                // -0x278/-0x274/-0x270 **连续**槽（= npc 对象本体），随后
                // 0x08a82007 pair<int&,stDungeonNpc&> 两实参**同址** -0x278
                // ⇒ 原码直写对象字段（P329：无 key/npcIdx/rate 具名中转）。
                // 旧形（int key/npcIdx/rate + 拷贝）在 0x08a81f? 处多 6 条
                // load/store（我方 -0x294/-0x290/-0x28c 目标槽 + -0x27c 源槽）。
                stDungeonNpc npc;
                npc.m_field0 = ScanInt(&flag);
                if (!flag)
                    break;
                npc.m_npcIdx = ScanInt((bool*)0);
                npc.m_rate = ScanInt((bool*)0);
                for (int i = 0; i < 4; ++i)
                    ScanInt((bool*)0);
                pScript->m_levelNpcMap.insert(
                    std::pair<int, stDungeonNpc>(npc.m_field0, npc));
            }
            pScript->verifyNpc(pScript->m_levelNpcMap);
        }
        else if (token == "[npc]")
        {
            stNpcSales sales;
            sales.m_field0 = ScanInt((bool*)0);   // npcKey
            // ORIG 内层检查顺序： [dungeon] → [level] → [dungeon cash item] → [/npc]
            // （P46 叶块序），ScanType 返回值丢弃（ORIG 直接调用）。
            for (;;)
            {
                ScanType(token, true);
                if (token == "[dungeon]")
                {
                    stDungeonSales ds;
                    ds.m_field0 = ScanInt((bool*)0);       // dungeonKey
                    ds.m_maxCount = ScanInt((bool*)0);     // +0x04
                    for (;;)
                    {
                        // [M5D_SecretShop] ORIG 0x08a8211c：带 flag 的 ScanInt 直落
                        // sale 对象首字段槽 -0x290（m_rule 见 0x08a82145
                        // `mov %al,-0x28c` = 对象内 +4），0x08a82193/0x08a8219d
                        // pair<int&,stSaleInfo&> 两实参**同址** -0x290 ⇒ 直写对象 +
                        // pair(sale.m_itemIdx, sale)（P329：删 itemIdx 具名中转，
                        // 旧形多 2 条 load/store）。
                        stSaleInfo sale;
                        sale.m_itemIdx = ScanInt(&flag);
                        if (!flag)
                            break;
                        sale.m_rule = (char)ScanInt((bool*)0);
                        sale.m_price = ScanInt((bool*)0);
                        sale.m_materialCount = ScanInt((bool*)0);
                        sale.m_limit = ScanInt((bool*)0);
                        sale.m_rate = ScanInt((bool*)0);
                        ds.m_sales.insert(std::pair<int, stSaleInfo>(sale.m_itemIdx, sale));
                    }
                    sales.m_dungeonSales.insert(
                        std::pair<int, stDungeonSales>(ds.m_field0, ds));
                }
                else if (token == "[level]")
                {
                    stDungeonSales ds;
                    ds.m_field0 = ScanInt((bool*)0);
                    ds.m_maxCount = ScanInt((bool*)0);
                    for (;;)
                    {
                        // [M5D_SecretShop] ORIG 0x08a82339：同 [dungeon] 形（sale 槽
                        // -0x2a8，0x08a823ab/0x08a823b5 pair 两实参同址 -0x2a8）。
                        stSaleInfo sale;
                        sale.m_itemIdx = ScanInt(&flag);
                        if (!flag)
                            break;
                        sale.m_rule = (char)ScanInt((bool*)0);
                        sale.m_price = ScanInt((bool*)0);
                        sale.m_materialCount = ScanInt((bool*)0);
                        sale.m_limit = ScanInt((bool*)0);
                        sale.m_rate = ScanInt((bool*)0);
                        ds.m_sales.insert(std::pair<int, stSaleInfo>(sale.m_itemIdx, sale));
                    }
                    sales.m_eventSales.insert(
                        std::pair<int, stDungeonSales>(ds.m_field0, ds));
                }
                else if (token == "[dungeon cash item]")
                {
                }
                else if (token == "[/npc]")
                {
                    pScript->m_dungeonNpcMap.insert(
                        std::pair<int, stNpcSales>(sales.m_field0, sales));
                    break;
                }
            }
        }
        else if (token == "[event item]")
        {
            pScript->m_eventDungeonSales.m_maxCount = ScanInt((bool*)0);  // +0x64
            for (;;)
            {
                // [M5D_SecretShop] ORIG 0x08a8264b：带 flag ScanInt 直落 sale 槽
                // -0x2c0（0x08a826bf/0x08a826c9 pair 两实参同址 -0x2c0）。
                stSaleInfo sale;
                sale.m_itemIdx = ScanInt(&flag);
                if (!flag)
                    break;
                sale.m_rule = (char)ScanInt((bool*)0);
                sale.m_price = ScanInt((bool*)0);
                sale.m_materialCount = ScanInt((bool*)0);
                sale.m_limit = ScanInt((bool*)0);
                sale.m_rate = ScanInt((bool*)0);
                pScript->m_eventDungeonSales.m_sales.insert(
                    std::pair<int, stSaleInfo>(sale.m_itemIdx, sale));
            }
        }
        else if (token == "[price random]")
        {
            pScript->m_priceVarPercent = ScanInt((bool*)0);   // +0x80
        }
        else if (token == "[eplp wait time]")
        {
            pScript->m_int84 = ScanInt((bool*)0);             // +0x84
        }
        else if (token == "[add slot cash user stackable count]")
        {
            // ORIG 0x08a82794 `jne 08a81d4e`（nop 岛 → jmp cond）⇒ 原码是
            // 空处理 + continue（P223 continue 岛）。
            continue;
        }
        else if (token == "[secret shop end]")
        {
            // ORIG 0x08a827b8 `movb $0x1,-0x251(%ebp)` ⇒ 置 flag 后 break；
            // 函数返回值即该 flag（ORIG 尾 0x08a827c2 `movzbl -0x251,%ebx` +
            // `mov %ebx,%eax`），旧实现恒 return 1 属真 bug。
            flag = true;
            break;
        }
    }
    return flag;
}