// ---- GetOneADayItemList @ 0x8513076 ----
std::vector<std::pair<int, int> >& CItemShop::GetOneADayItemList() const
{
    return const_cast<CItemShop*>(this)->m_oneADayList;
}