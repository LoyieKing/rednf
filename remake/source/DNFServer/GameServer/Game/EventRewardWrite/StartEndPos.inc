// ORIG 以 this+0x18 内嵌 CSlotBoundChecker 调用 get_slot_bound；重建 CDataManager.h
// 以同布局 map 成员 m_slotBoundMap 保持 ABI（CSlotBoundChecker 静态实现无成员），
void eventReward::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end,
                              int cap) const
{
    if (!reinterpret_cast<EventRewardCheckerPos const*>(this)->m_checker.get_slot_bound(
            type, cap, start, end))
    {
        start = 0;
        end = 0;
    }
}