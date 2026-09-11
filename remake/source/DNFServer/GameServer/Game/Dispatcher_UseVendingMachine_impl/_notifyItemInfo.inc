// ORIG T 0x0821c17c _ZNK28Dispatcher_UseVendingMachine15_notifyItemInfoEPK22stVendingMachineInfo_tP5CUserRKNS_17stPutItemResult_tE
//   在 info->m_lottery（vector<pair<int,int>>）中查找 (itemIdx,count)；命中才
//   registTimer 延迟发放（ORIG _notifyItemInfo 汇编：pair 构造 + find + operator==
//   判等后跳 registTimer）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：pair 直传 result 字段（去 (int)
//   cast ⇒ 复现 pair<unsigned long const&,unsigned int const&> 转换构造）、m_lottery
//   内联 begin/end（去具名 lot 引用）、具名 it + `== end` + goto 尾空标签复现
//   ORIG 0x0821c21f `jne <nop>` 岛（P223）。
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
}