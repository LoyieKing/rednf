Inven_Item Dispatcher_UseVendingMachine::_getGiveItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;           // 初始 idx = -1（“未选择”标记）
    unsigned long range = info->m_giveCount - 1;
    unsigned long pick = m_rand->randInt(range);
    for (std::vector<stVendingMachineOutput_t>::const_iterator it = info->m_vecGive.begin(),
             e = info->m_vecGive.end(); it != e; ++it)
    {
        if (pick < it->m_cumWeight)
        {
            out.m_addInfo = it->m_itemId;
            out.m_addInfo2 = (int)it->m_count;
            return out;
        }
    }
    return out;
