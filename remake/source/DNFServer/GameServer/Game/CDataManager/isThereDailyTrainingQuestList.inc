bool CDataManager::isThereDailyTrainingQuestList(int idx, int questIdx)
{
    if (idx <= 0 || idx > 0x46)
    {
        return 0;
    }
    std::vector<int>* list = getDailyTrainingQuest(idx);
    for (std::vector<int>::iterator it = list->begin(); it != list->end(); ++it)
    {
        if (*it == questIdx)
        {
            return 1;
        }
    }
    return 0;
