Inven_Item Dispatcher_UseVendingMachine::_getBonusItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;
    // ORIG 0x0821c058：GetRepeatEvent(0x23) → IsEventing(0) 判活，
    // 事件未开启 → 返回空 Inven_Item（m_addInfo=-1）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x23)->IsEventing(0))
        return out;
    if (info->m_vecBonus.empty())
        return out;
    unsigned long range = info->m_bonusCount - 1;
    unsigned long pick = m_rand->randInt(range);
    for (std::vector<stVendingMachineOutput_t>::const_iterator it = info->m_vecBonus.begin(),
             e = info->m_vecBonus.end(); it != e; ++it)
    {
        if (pick < it->m_cumWeight)
        {
            out.m_addInfo = it->m_itemId;
            out.m_addInfo2 = (int)it->m_count;
            return out;
        }
    }
    return out;
