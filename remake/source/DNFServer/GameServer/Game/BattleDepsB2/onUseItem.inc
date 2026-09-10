void CBattle_Field::onUseItem(CUser* user, const Inven_Item& item)
{
    MapInfo* mapInfo = GetCurrentMapInfo();

    if (item.m_addInfo == 0xbdd)                 // +0x02 dword（技能材料物品）
    {
        // P0：ORIG tick 无槽（call 结果 eax 直通实参），内联右值。
        mapInfo->m_killChecker1.setUseSkillMaterial(
            GlobalData::s_systemTime_.getCurTickCount());   // +0x4c
        mapInfo->m_killChecker2.setUseSkillMaterial(
            GlobalData::s_systemTime_.getCurTickCount());   // +0x64
    }

    if ((unsigned char)item.m_field1 == 2)
        ++mapInfo->m_playInfo.m_useItemType2Count;          // +0x84
    else
        ++mapInfo->m_playInfo.m_useItemOtherCount;          // +0x88
