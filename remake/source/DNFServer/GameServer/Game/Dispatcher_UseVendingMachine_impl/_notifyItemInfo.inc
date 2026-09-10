void Dispatcher_UseVendingMachine::_notifyItemInfo(
    const stVendingMachineInfo_t* info, CUser* user,
    const stPutItemResult_t& result) const
{
    std::pair<int, int> key(result.m_itemIdx, result.m_count);
    std::vector<std::pair<int, int> >::const_iterator it =
        std::find(info->m_lottery.begin(), info->m_lottery.end(), key);
    if (it == info->m_lottery.end())
        goto done;
    TimerVendingMachineItemNotify::registTimer(user->GetUID(), result.m_itemIdx,
                                               result.m_count, 5);
done:;
