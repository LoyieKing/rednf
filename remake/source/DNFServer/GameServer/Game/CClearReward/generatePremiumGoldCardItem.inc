// [b28d-solo-CClearReward] verdict=NEAR（N1 封顶）：102=102。唯一残差 = 概率表
// 基址常量折叠——ORIG `add $0x1264,%edx; mov 0x14(%eax,%edx,4),%eax`（两步）vs
// 4.4 折 `add $0x1269,%edx; mov (%eax,%edx,4),%eax`（一步；探针 p6 f1/f2/f6 穷举）。
// [L5e-CargoClearReward 2026-09-11 复验·负结论] 再试「函数内局部视图分离基址」形
// `struct { char m_head[0x14]; int m_probs[0x1264+6]; }` + `m_probs[i+0x1264]`：
// 实测 102→103 反向劣化（`add $0x1264,%edx` + `add $0x4,%edx` +
// `mov 0x4(%eax,%edx,4),%eax`）⇒ 4.4 对 scaled-index 寻址恒把 disp 中 4 的倍数
// 部分折成**额外 add**（0x14→索引 +4 / 余 0x4 留 disp），永不保留 ORIG 的 disp 0x14。
// 结合 p6 六形穷举 ⇒ 该件 N1 封顶确证，禁再试基址侧旋钮。
void CClearReward::generatePremiumGoldCardItem(
    stGenerateRefData_t const& ref, stGenerateResult_t& result,
    ENUM_RARITY rarity, unsigned int defaultItem)
{
    // [b28d-solo-CClearReward] ORIG 0x08538a99 函数头两条初始化
    // （`movl $0x2,-0x20(%ebp)` / `movl $0x0,-0x1c(%ebp)`）⇒ 原码 dropType/rarityIdx
    // 均为函数头具名局部（P0 槽往返律），调用点读 -0x20/-0x1c 而非内联立即数。
    eItemDropType_t dropType = eItemDropType_2;
    ENUM_RARITY rarityIdx = static_cast<ENUM_RARITY>(0);
    if (static_cast<int>(rarity) < 0)
    {
        int roll = m_random.generateNumber(3, 10000);
        for (int i = 0; i < 6; ++i)
        {
            // 视图 + 内联索引：ORIG `mov 0x14(%eax,%edx,4),%eax` 单条 scaled 装载
            // （指针局部会多占槽并使基址链改形，P0）。
            if (roll <= reinterpret_cast<CDataManagerRarityProbView*>(
                            G_CDataManager())->m_probs[i + kRarityProbIdx])
            {
                rarityIdx = static_cast<ENUM_RARITY>(i);
                break;
            }
        }
    }
    else
    {
        rarityIdx = rarity;
    }
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        dropType, rarityIdx);
    int itemIdx = m_table.chooseItem(
        dropType, rarityIdx,
        static_cast<unsigned char>(ref.m_field34));   // §4.3 具名化（+0x34）
    if (itemIdx == -1)
        itemIdx = static_cast<int>(defaultItem);
    // ORIG `mov %eax,-0x14(%ebp); cmpl $0x0,-0x14(%ebp); je` ⇒ 原码为具名结果局部
    // 后内存判空（P115），非调用表达式直比（后者 4.4 产 sete/test 物化）。
    CItem* found = G_CDataManager()->m_itemList->find_item(itemIdx);
    if (found == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}