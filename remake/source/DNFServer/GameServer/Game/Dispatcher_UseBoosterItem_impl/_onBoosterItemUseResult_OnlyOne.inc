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
