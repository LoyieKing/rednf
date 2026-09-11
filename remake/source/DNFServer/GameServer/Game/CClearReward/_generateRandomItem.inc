// [b28d-solo-CClearReward] verdict=DIFF（N 封顶）：
// 已修 —— m_dropProb 枚举化（P254 单段直落）、数组指针形 (*(m_workProbs+drop))[idx]
// （P30/P183 钥匙，探针 p5 f1/f2）、dropRoll 内联（P0）、thr 改 A*(1.0f-dropRate)
// 书写序（P243）、find_item 具名结果（P115）。
// 残余：①ORIG 把 e.v[0]/v[1]/v[2] 三字段各存槽（-0x3c/-0x38/-0x34）而我方只存
// e 地址（-0x40）——「三具名 int 局部（v0/v1 后续未用）」不可由合法源形反推；
// ②帧 0xa4 vs 0x9c（push edi/esi）+ 全帧槽号平移（N82）；③itemIdx/found/循环变量
// 槽位连锁。
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
    // [L5e 2026-09-11 负结论] 按值拷贝形（`stGenItemProb_t e = …;`，复现 ORIG
    // 0x08538822 三槽拷贝 `mov (%eax),%edx; mov %edx,-0x3c; …`）实测指令数
    // 202→206（gap 0→−4）：三槽拷贝本身对齐，但 4.4 在后续 x87 段/寄存器分配上
    // 多出 4 条（ebx 缓存链被拉长），整体劣化 ⇒ 保持引用形（P0 判读：ORIG 的三槽
    // 属 4.1 结构体拷贝产物，4.4 同构源码恒发不同形）。
    const stGenItemProb_t& e = (*(m_workProbs + refClear.m_dropProb))[idx];
    // ORIG 两处均 fistpl 截断（fnstcw/fldcw 0xc 截断模式），非 round；
    // 保持 (int) 强转截断语义。ORIG 的 x87 序：先算整数因子 A，再 fld1/fsubs
    // 得 (1.0f-dropRate) 并 fmulp（P243 书写序 ⇒ A * (1.0f-dropRate)）。
    // [L5e 2026-09-11] ORIG 0x08538833 段序 = `fildl v2`（0x0853883c）先、
    // `flds 0x4(refClear)`（0x08538845）后 ⇒ 内层乘法的**左操作数是 e.v[2]**
    // （P243：首条 fildl/flds 即代码里先书写的操作数）。
    int thr = static_cast<int>(
        static_cast<float>(
            (static_cast<int>(e.v[2] * refClear.m_bonusRate) +
             m_dungeonDropBonus[refClear.m_level]) *
            (param / 100)) *
        (1.0f - dropRate));

    // ORIG 0x085388ca：generateNumber(0,10000) 结果**不落槽**（`cmp -0x2c(%ebp),%eax;
    // setge %al; test; jne` 直判）⇒ 内联实参（P0）。
    if (m_random.generateNumber(0, 10000) >= thr)
        return;

    // [L5e 2026-09-11] ORIG 0x08538882 段两条初值 store 序为
    // `movl $0x0,-0x28`（cum）先、`movl $0x1,-0x24`（dropType）后，且 cum 占更深槽
    // ⇒ 原码声明序 = cum 在前（4.4 按声明序发 store 且先声明占深槽）。
    int cum = 0;
    int dropType = 1;   // 初值（未命中即 1）
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
}