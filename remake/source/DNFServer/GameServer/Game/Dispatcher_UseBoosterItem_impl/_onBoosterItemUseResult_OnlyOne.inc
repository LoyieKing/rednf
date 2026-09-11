// _onBoosterItemUseResult_OnlyOne(CUser*, vector<Inven_Item>&, int) —— ORIG 0x08209a5e
// b28d-60：P277 receiver cast 前置——原写法 `static_cast<GrowthEquipEvent*>(evt2)->f(args)`
// 恒 this-first（receiver 先入 %eax、实参经 %edx）；把 cast 移到赋值点（具名派生指针）
// 复现 ORIG 的「实参先装 %eax、receiver 最后压栈」序。
void Dispatcher_UseBoosterItem::_onBoosterItemUseResult_OnlyOne(CUser* user, std::vector<Inven_Item>& items, int itemId)
{
    if (items.size() != 1)
        return;

    std::vector<Inven_Item>::iterator it = items.begin();

    // ORIG：GetRepeatEvent(100) 结果直接虚调 vtable+0x34（IsEventing(0)），无判空；
    // 命中后才二次 GetRepeatEvent(100)（存 -0xc 槽）并判空。
    if (static_cast<GrowthEquipEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(100))->IsEventing(0))
    {
        GrowthEquipEvent* evt2 =
            static_cast<GrowthEquipEvent*>(GlobalData::s_event_manager->GetRepeatEvent(100));
        if (evt2 != 0)
            evt2->SelectGrowthWeapon(*user, itemId, it->m_addInfo);
    }
}