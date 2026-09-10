int CharacAdvanceAltarManager::setSlot(bool isSet, const CMDPacketStruct::_SetSlotData& data)
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
    if (buyIt != items->end())
    {
        return 3;                        // 已购买过
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
