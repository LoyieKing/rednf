// ---------------------------------------------------------------------------
// _generateRandomItem — ORIG T 0x08536882（CSpecialMonsterDrop，报告分段转写；
// 生成率三表偏移 0xa8/0xf8/0x15c 与 loadScript 写入证据一致）。
// ---------------------------------------------------------------------------
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N85/N93）→ 试=新口径 n1_v 364/365 gap=-1 blocks=57：ORIG -0x48/-0x64.. 槽 vs 我方 -0x50/-0x6c..（全槽平移 8）+ x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp，N85）⇒ 维持。
void CSpecialMonsterDrop::_generateRandomItem(unsigned char type, char role,
                                              unsigned char dungeonLv,
                                              unsigned char difficulty,
                                              unsigned char partyPos,
                                              int bourgeoisRate,
                                              unsigned char hellFlag,
                                              int dropRate,
                                              stGenerateResult_t& result,
                                              bool& done)
{
    // [b28d-solo-WongWorkDrop_B] 语义完成 DIFF（364/370）。
    // 修法 = --partyPos 直接改形参槽（ORIG `subb $1,-0x1dc`）+ 花括号零初始化
    // （P214③/P253）+ 去 boost 声明处初始化 + e.w 视图 + 内联实参。
    // 残差 N 封顶：①帧/全槽号平移 0x10（0x21c vs 0x22c，N82/N93 类）；
    // ②x87 比较 lowering——ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp
    //   （N85；P230 提示 -march=i486 可消，属路由变更，已上报 Main）。
    // [M3S2WinDrop 2026-09-11] 本波落地 2 项（370→365，gap +6→+1；四数不变）：
    //   ① UseLuckPoint 三实参改**全内联**（去具名 grade/itemRarity 槽）——ORIG 由
    //      `call get_grade; mov %eax,%esi` / `call get_rarity; mov %eax,%ebx` 用
    //      callee-saved 保活 ⇒ -3 条；
    //   ② 去 probAcc 具名（乘法直接写在 cast 内）——ORIG 0x085369b3 由 x87 栈上值
    //      直接 `fnstcw/fldcw/fistpl`，无 float 槽往返 ⇒ -2 条。
    // 残余（判 N，编译器差异）：③ `t.v[2+i]` 我方多 1 条 `add $0x2,%eax`（ORIG
    //   `mov -0x5c(%ebp,%eax,4)` 直接以 eax=i 索引，4.4 不折叠常量基址）；
    //   ④ 后半段实参缓冲 ORIG 用 ebx/ecx/edx 而 4.4 用槽 + esi/edi（同条数）；
    //   ⑤ x87 比较 lowering（N85）；⑥ `jl` vs 4.4 `jg`（等价反向）。
    int probIdx = _getGenItemProbIdx(static_cast<int>(dungeonLv));
    if (probIdx < 0)
        return;

    stGenItemProb_t t = m_probsSub[probIdx];         // ORIG 拷贝 7 ints 到栈
    // ORIG 直接对形参 spill 槽做 `subb $0x1,-0x1dc(%ebp)`（非另起 int 局部）。
    --partyPos;

    for (int i = 1; i <= 4; ++i)
    {
        float factor = 1.0f;
        // [b28d-38] P214③/P253：ORIG 内联清零（无 memset call）⇒ 花括号零初始化。
        int fallbackTable[6] = {0};                  // ORIG 栈 -0x7c..-0x64（24B 清零）
        int* tablePtr = &m_rarityBasis[i][0];
        double boost;

        if (i == 2)
        {
            // [b28d-38] P183/P167：ORIG 无 amc 具名局部，两次
            // `G_CDataManager()->GetAutoMarketContitionsControl()` 各重发一次调用。
            factor = G_CDataManager()->GetAutoMarketContitionsControl()
                         ->GetItemDropRate(probIdx);
            boost = 0.0;
            if (factor != 0.0f)
                boost = 1.0f / factor - 1.0f;
            if (G_CDataManager()->GetAutoMarketContitionsControl()
                    ->ChangeItemRarity(&m_rarityBasis[i][0], fallbackTable, 6,
                                       boost))
                tablePtr = fallbackTable;
        }

        // prob = trunc( v[2+i] × factor × dungeonBonus[i][difficulty](0xf8)
        //             × monsterTypeBonus[i][role](0x15c)
        //             × partyBonus[i][ppIndex](0xa8) ) —— ORIG 乘法顺序
        // [M3S2WinDrop] 去 probAcc 具名：ORIG 0x085369b3 由 x87 栈上值直接 `fnstcw/
        // fldcw/fistpl`（无 float 槽往返）；具名 float 会让 4.4 多 `fstps S; flds S` 2 条。
        int probValue = static_cast<int>(
            static_cast<float>(t.v[2 + i]) * factor *
            m_dungeonBonus[i][difficulty] *
            m_monsterTypeBonus[i][static_cast<int>(role)] *
            m_partyBonus[i][partyPos]);              // fldcw truncate ≡ (int)
        int dropValue = probValue * dropRate / 100;  // 魔数除法 = signed /100

        int randValue = m_random.generateNumber(0, 10000);
        if (!(randValue <= dropValue))               // ORIG jl skip
            continue;

        int luckRoll = m_random.generateNumber(1, 1000000);
        int rarity =
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)->GetItemRarity(
                tablePtr, luckRoll, i);

        CItemGeneratorMgr* mgr = G_CDataManager()->getItemGenerator();
        mgr->onTryGenerateRandomItem(CItemGeneratorMgr::eGenerateType_0,
                                     static_cast<eItemDropType_t>(i),
                                     static_cast<ENUM_RARITY>(rarity));

        int itemIdx =
            m_table.chooseItem(static_cast<eItemDropType_t>(i),
                               static_cast<ENUM_RARITY>(rarity),
                               static_cast<int>(type),
                               static_cast<ENUM_DUNGEON_DIFFICULTY>(difficulty),
                               static_cast<ENUM_ITEM_DROP_DUNGEON_TYPE>(hellFlag));
        if (itemIdx == -1)
            continue;

        CItem* item = G_CDataManager()->m_itemList->find_item(itemIdx);   // [b28d-38] 同上
        if (item == 0)
            continue;

        if (i == 2)
        {
            // [b28d-38] P253：ORIG `rep stos`（0x40 dwords）⇒ 花括号零初始化。
            char buf[256] = {0};                      // ORIG -0x1b9 帧 256B 清零
            sprintf(buf,
                    "special monster drop rate : item_id(%d), mon_lv(%d), "
                    "role(%d), dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv), static_cast<int>(difficulty),
                    static_cast<int>(partyPos),
                    bourgeoisRate, dropRate, randValue, dropValue);
            reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                ->sendItemDropRate(buf);
        }

        // ORIG 同型四连 + UseLuckPoint：Inven_Item ctor / 写 item idx /
        // create_item(1,&inv,1) / get_grade+get_rarity+UseLuckPoint / push_back。
        Inven_Item inv;                                // ORIG 内联 C2
        inv.m_addInfo = itemIdx;                       // ORIG 写对象首 +0x02
        G_CDataManager()->m_itemList->create_item(
            static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
        // [M3S2WinDrop] ORIG 0x08536b?? 三实参均内联：`call get_grade; mov %eax,%esi`
        // / `call get_rarity; mov %eax,%ebx`（callee-saved 保活，无具名槽）。
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
            ->UseLuckPoint(static_cast<int>(inv.m_field1), item->get_rarity(),
                           item->get_grade(),
                           static_cast<int>(result.m_dropItems.size()));
        result.m_dropItems.push_back(inv);
        done = true;                                  // movb $1,(p10)
    }
}