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
