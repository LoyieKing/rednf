// ORIG T 0x0821c058 _ZNK28Dispatcher_UseVendingMachine13_getBonusItemEPK22stVendingMachineInfo_t
//   依 m_bonusCount 权重随机选 m_vecBonus 中一个输出（事件开启检查见上版推断注释）。
//   命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：事件判活改内联链式
//   `s_event_manager->GetRepeatEvent(0x23)->IsEventing(0)`（ORIG 无 ev 判空/无具名槽，
//   0x0821c071 段），range 防御删除、迭代器循环同 _getGiveItem。
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
}