std::vector<int>* CDataManager::getDailyTrainingQuest(int idx)
{
    return &m_dailyTrainingQuest[idx - 1];
