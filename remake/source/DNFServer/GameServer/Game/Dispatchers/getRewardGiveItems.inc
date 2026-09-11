// ===== DimensionActivationRewardData::getRewardGiveItems —— ORIG 0x08915f9a =====
// ORIG：list=getRewardGiveItemList(job, growType)；list 非空且
//   selectionIdx>=0 且 selectionIdx<=list->size()（ORIG cmp/jl 边界）→
//   返回 &list->at(selectionIdx)（ORIG 调 at，idx==size 时由 at 抛异常）；否则 0。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x08915f9a orig/ours=34/34
std::vector<EventRewardItemInfo>* DimensionActivationRewardData::getRewardGiveItems(
    int selectionIdx, int job, int growType)
{
    std::vector<std::vector<EventRewardItemInfo> >* list =
        getRewardGiveItemList(job, growType);
    if (list != 0 && selectionIdx >= 0 && selectionIdx <= (int)list->size())
    {
        return &list->at(selectionIdx);
    }
    return 0;
}