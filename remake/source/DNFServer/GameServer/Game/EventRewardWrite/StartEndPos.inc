void eventReward::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end,
                              int cap) const
{
    if (!reinterpret_cast<EventRewardCheckerPos const*>(this)->m_checker.get_slot_bound(
            type, cap, start, end))
    {
        start = 0;
        end = 0;
    }
