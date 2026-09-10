void CClearReward::_generateRandomItem(stGenerateRefClearReward_t const& refClear,
                                       int param, stGenerateResult_t& result,
                                       float dropRate)
{
    // 第一参 = refClear.m_value（命中基准）；档位 = refClear.m_dropProb。
    int idx = _getGenItemProbIdx(
        static_cast<int>(refClear.m_value),
        refClear.m_dropProb);
    if (idx < 0)
        return;

    // ORIG 0x085387e5：`lea 0x60(%eax),%ebx; …; lea (%ebx,%eax,1),%edx` —— 数组名
    // 退化指针 + 下标形（同 _getGenItemProbIdx；探针 p5 f1/f2 实证）。
    const stGenItemProb_t& e = (*(m_workProbs + refClear.m_dropProb))[idx];
    // ORIG 两处均 fistpl 截断（fnstcw/fldcw 0xc 截断模式），非 round；
    // 保持 (int) 强转截断语义。ORIG 的 x87 序：先算整数因子 A，再 fld1/fsubs
    // 得 (1.0f-dropRate) 并 fmulp（P243 书写序 ⇒ A * (1.0f-dropRate)）。
    int thr = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(refClear.m_bonusRate * e.v[2]) +
             m_dungeonDropBonus[refClear.m_level]) *
            (param / 100)) *
        (1.0f - dropRate));

    // ORIG 0x085388ca：generateNumber(0,10000) 结果**不落槽**（`cmp -0x2c(%ebp),%eax;
    // setge %al; test; jne` 直判）⇒ 内联实参（P0）。
    if (m_random.generateNumber(0, 10000) >= thr)
        return;

    int dropType = 1;   // 初值（未命中即 1）
    int cum = 0;
    int roll = m_random.generateNumber(2, 10000);
    for (int i = 1; i <= 4; ++i)
    {
        cum += m_dropItemTypeProb[i - 1];
        if (roll < cum)
        {
            dropType = i;
            break;
        }
    }
    int rarityRoll = m_random.generateNumber(3, 1000000);
    // ORIG：CLuckPoint::GetItemRarity(this=result+0xc)——全局类与本域 CLuckPoint
    // 前 0xc（vector<CUser*>）布局一致，按 ORIG 同址处理。
    typedef ::CLuckPoint GlobalLuckPoint;
    int rarity = reinterpret_cast<GlobalLuckPoint*>(&result.m_luckPoint)
                     ->GetItemRarity(m_rarityBasis, rarityRoll, dropType);
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        static_cast<eItemDropType_t>(dropType),
        static_cast<ENUM_RARITY>(rarity));
    int itemIdx = m_table.chooseItem(static_cast<eItemDropType_t>(dropType),
                                     static_cast<ENUM_RARITY>(rarity),
                                     refClear.m_value);
    if (itemIdx == -1)
        return;
    // ORIG `mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); je` ⇒ 具名结果局部（P115）。
    CItem* found = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (found == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
