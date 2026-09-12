// ---- ORIG T 0x08131b58（setSlot.md 全文）----
// [b28d-solo-advancealtar_Manager AE] 本次修：shopType 派发改 switch（ORIG 平铺
// `cmp $1; je; cmp $2; je; jmp` 决策链 + 各 case 内联字段检查）+ upgrades 循环
// const_iterator→iterator（ORIG IPN 实例）+ 每用点重取 operator->（P48）+
// items 侧 const map 视图 + 具名 find 迭代器（ORIG find-先/end-后 + const eq）+
// 两遍 slots 扫描共用同一 it（ORIG 同槽 -0x44，第二遍 `it = begin()` 经临时）⇒
// 274=274 逐条全等。p167：items 处 ORIG 用 const_iterator::eq 而我方 `!=` 发 ne
// （语义等价模板实例差；`!(==)` 形实测 275 vs 274 劣化，保 AE）。
// [g09 2026-09-11 P211 callee 侧旋钮·Main 已批准] 返回 int→unsigned int（签名行同步，
// 函数体不动）：唯一消费点 Dispatcher_SetSlot::process_packet 的 ORIG store 为
// LHS-first 2 条无中转 ⇒ 与 _ResAdvanceAltarSetSlot::m_nErrorCode(unsigned) 同型。
unsigned int CharacAdvanceAltarManager::setSlot(bool isSet, const CMDPacketStruct::_SetSlotData& data)
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 2;
    }
    AdvanceAltarShopType::T shopType;
    // [b28d-solo-advancealtar_Manager] ORIG 平铺决策链
    // `cmp $1; je body1; cmp $2; je body2; jmp else`（4.1 对 if/else-if 的正向布局）
    // ⇒ 试 switch 形（P46/P251：叶块序=case 书写序）。
    switch (data.m_type)
    {
    case 1:
        shopType = AdvanceAltarShopType::T_0;
        // [b28d-solo-advancealtar_Manager] ORIG +0x36 case 块内联字段检查
        // （每个 case 各一份 `cmp $0x5; jbe`，非汇聚单份）⇒ 照抄原码重复写。
        if (static_cast<unsigned char>(data.m_field4) > 5)
        {
            return 2;
        }
        break;
    case 2:
        shopType = AdvanceAltarShopType::T_1;
        if (static_cast<unsigned char>(data.m_field4) > 5)
        {
            return 2;
        }
        break;
    default:
        return 2;
    }

    std::vector<BuyUpgradeData>* upgrades =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    if (upgrades == 0)
    {
        return 3;
    }
    const BuyUpgradeData* chosen = 0;
    // [b28d-solo-advancealtar_Manager] ORIG 调用 non-const iterator 族
    // （ptEv/deEv/ppEi 均 IPN... 实例）⇒ const_iterator 会产生另一组模板实例（P96）。
    for (std::vector<BuyUpgradeData>::iterator it = upgrades->begin();
         it != upgrades->end(); it++)
    {
        if (it->m_nItemId == data.m_field8)
        {
            chosen = &(*it);
            break;
        }
    }
    if (chosen == 0)
    {
        return 3;
    }
    if (chosen->m_field8 == 0)          // “is settable slot”未开放
    {
        return 3;
    }
    if (!isSet)
    {
        return resetSlot(*cur, data);   // ORIG 尾 jmp 进入 resetSlot 共用出口
    }

    // [b28d-solo-advancealtar_Manager] ORIG find/end 走 const 版
    // （_ZNKSt3map...4findERS5_ / _ZNKSt3map...3endEv，eq 为
    //  _Rb_tree_const_iterator 版）⇒ 形参侧 const map 视图；且 find 结果具名
    // 直构（P74②：命名 it 固定 find-先/end-后求值序，单表达式 4.4 恒 end 先）。
    const std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }
    std::map<int, _BuyItem>::const_iterator buyIt = items->find(data.m_field8);
    // [b28d-solo-advancealtar_Manager] ORIG 该点用 const_iterator::eq（p167
    // MISSING eq / EXTRA ne）；`!(==)` 形实测 275 vs 274（多 1 条）劣于 `!=` 的
    // AE 274=274 ⇒ 保 `!=`（语义等价，模板实例差记 N）。
    // [SemB08 2026-09-12 真语义修正（推翻上判）] ORIG 0x08131d5c 段逐条直证：
    //   `call _ZNKSt23_Rb_tree_const_iteratorISt4pairIKiN12advancealtar8_BuyItemEEEeqERKS5_`
    //   → `test %al,%al` → `je <+0x210 继续主流程 getSlotMap>` → `mov $0x3,%eax` → `jmp <epilogue>`。
    //   %al 是 `(it == end)` ⇒ 语义为「**未找到才** return 3」，我方原写 `!=`（找到才 return 3）
    //   **方向相反**。p167 的 Counter 口径对「eq↔ne 同计数换名」失明，历史判词「模板实例差」
    //   因此错误；按 ORIG 改回 `==`（同时消 MISSING eq / EXTRA ne）。
    if (buyIt == items->end())
    {
        return 3;                        // 未购买过（ORIG 语义）
    }

    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    // [b28d-solo-advancealtar_Manager] ORIG 两遍扫描共用同一 it 槽（-0x44）：
    // 第一遍直构（begin sret 落 it），第二遍 `it = slots->begin();` 经 -0x50 临时
    // 再拷回 it ⇒ 源码为函数级声明 + 第二遍赋值（P50 复活）。
    std::map<unsigned char, _Slot>::iterator it = slots->begin();
    for (; it != slots->end(); it++)
    {
        if (it->second.m_int6 == data.m_field8)
        {
            return 4;                    // 同一物品已在其它槽装备
        }
    }
    it = slots->begin();
    for (; it != slots->end(); it++)
    {
        // [b28d-solo-advancealtar_Manager] ORIG 每用点重取 operator->（P48/P157
        // 去引用缓存：三处 ptEv 各自发 call，不缓存 &second）。
        if (it->second.m_byte5 == static_cast<unsigned char>(data.m_field4) &&
            it->second.m_int1 == data.m_type &&
            it->second.m_byte0 != 1)           // 未锁定
        {
            it->second.m_int6 = data.m_field8;        // ORIG it+7 ← data+8
            cur->m_update = 1;
            return 0;
        }
    }
    return 2;
}