// [b28d-solo-CDataManager] 返回类型 int→bool（P292/P168a 档①）：ORIG
// 0x0836411e 返回点 `mov $0/1,%eax`，唯一调用点 Quest::check_possible
// 0x08352f3b `call; xor $0x1,%eax; test %al,%al; je` ⇒ callee bool。
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
}