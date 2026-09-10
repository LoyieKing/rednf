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
        float probAcc = static_cast<float>(t.v[2 + i]) *
                        factor *
                        m_dungeonBonus[i][difficulty] *
                        m_monsterTypeBonus[i][static_cast<int>(role)] *
                        m_partyBonus[i][partyPos];
        int probValue = static_cast<int>(probAcc);   // fldcw truncate ≡ (int)
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
        int grade = item->get_grade();
        int itemRarity = item->get_rarity();
        reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
            ->UseLuckPoint(static_cast<int>(inv.m_field1), itemRarity, grade,
                           static_cast<int>(result.m_dropItems.size()));
        result.m_dropItems.push_back(inv);
        done = true;                                  // movb $1,(p10)
    }
