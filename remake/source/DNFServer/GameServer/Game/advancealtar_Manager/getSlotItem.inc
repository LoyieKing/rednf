// ---- ORIG T 0x08132638（getSlotItem.md 全文）。报告内 `local_78[0] =
// *(_SlotItem*)(slotIterElem+6)` 的 pair 基址记法按 ProcStage.h _SlotItem
// 写入点口径还原：itemId=等级点 FT_INDEX 的 FieldData::m_value、needGage=
// CItem::getAdvanceAltarNeedGage；键=槽表 first（槽号）。局部 tmp _SlotItem
// 构造自然发射 ORIG W 0x081347e2 的 C1/C2 弱符号。----
// [b28d-solo-advancealtar_Manager AE] 本次修：_SlotItem 构造提前到 find 检查后
// （ORIG +0x0a0）+ `slotItem.m_byte0 = sit->second.m_byte5`（ORIG +0x0ab movzbl
// 0x6）+ make_pair(slotItem.m_byte0, slotItem)（ORIG 两实参同址 &slotItem）+
// sit++/uit++（ppEi）+ 首个守卫正向包裹（P222/P257）⇒ 193=193 逐条全等。
// 真语义 bug：升级表匹配 ORIG 为 `uit->m_nItemId(+0x00)`，原实现误用 m_field4。
void CharacAdvanceAltarManager::getSlotItem(
    std::map<unsigned char, _SlotItem>& out) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    const std::map<unsigned char, _Slot>& slots = cur->m_slots1;      // ORIG base+0x84
    for (std::map<unsigned char, _Slot>::const_iterator sit = slots.begin();
         sit != slots.end(); sit++)          // ORIG +0x273 ppEi（后置，P28）
    {
        // [b28d-solo-advancealtar_Manager] ORIG +0x44 `cur+0x3c → -0x1c` 缓存
        // m_buyItems1 基址，find/end 共用（每轮重算一次）。
        const std::map<int, _BuyItem>& buyItems = cur->m_buyItems1;
        std::map<int, _BuyItem>::const_iterator bit =
            buyItems.find(sit->second.m_int6);                 // 槽内物品 id 查购买表
        // [b28d-solo-advancealtar_Manager] ORIG +0x09a `je <增量段>` ⇒ 正向包裹
        // （P222/P257：`if (bit != end) { body }` 产 je 跳过 body）。
        if (bit != buyItems.end())
        {
        // ORIG +0x0a0：_SlotItem 构造在 find 检查之后、upgrades 查询之前。
        _SlotItem slotItem;                                            // ORIG ctor memset(0xc)
        // ORIG +0x0ab `ptEv(sit); movzbl 0x6(%eax); mov %al,-0x74` ⇒ 槽号入 +0x00。
        slotItem.m_byte0 = sit->second.m_byte5;
        std::vector<BuyUpgradeData>* upgrades =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId,
                                                               AdvanceAltarShopType::T_0);
        if (upgrades == 0)
        {
            continue;
        }
        BuyUpgradeData* chosen = 0;
        // ORIG 循环后仍用 uit 作 receiver（+0x1a5 `lea -0x78; ptEv`）⇒ 声明提出。
        std::vector<BuyUpgradeData>::iterator uit = upgrades->begin();
        for (; uit != upgrades->end(); uit++)    // ORIG +0x156 ppEi（后置，P28）
        {
            // [b28d-solo-advancealtar_Manager 真语义 bug] ORIG +0x118
            // `ptEv(uit); mov (%eax),%ebx` = uit->m_nItemId(+0x00)，原实现误用
            // m_field4(+0x04)。
            if (uit->m_nItemId == bit->second.m_field4)
            {
                chosen = &(*uit);                     // ORIG 升级表匹配 *(buyIter+8)
                break;
            }
        }
        if (chosen == 0)
        {
            continue;
        }
        // ORIG +0x19e `movzwl 0xc(%eax); movswl %ax,%ebx` 后直传（P200：禁具名
        // 中间局部，拓宽即目标寄存器）。
        // ORIG +0x1a5 调用点 receiver 为 uit（`lea -0x78; ptEv`），非 chosen 缓存。
        FieldData* point = uit->getFieldDataPoint(bit->second.m_short8,
                                                  FieldType::FT_INDEX);
        if (point == 0)
        {
            continue;
        }
        const CItem* item = G_CDataManager()->find_item(point->m_value);
        if (item == 0)
        {
            continue;
        }
        slotItem.m_itemId = point->m_value;
        slotItem.m_needGage = item->getAdvanceAltarNeedGage();
        // ORIG +0x208 make_pair 两实参同址（&slotItem.m_byte0 / &slotItem）⇒
        // 键取 slotItem.m_byte0（= sit->second.m_byte5，与 sit->first 同值）。
        out.insert(std::make_pair(slotItem.m_byte0, slotItem));
        }
    }
}