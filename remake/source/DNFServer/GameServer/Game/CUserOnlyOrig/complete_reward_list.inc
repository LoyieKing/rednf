// W9 batch A: report-backed short accessors and container helpers.
void CUser::complete_reward_list(std::vector<std::pair<int, int> >& toDoList,
                                 std::vector<std::pair<int, int> >& doneList)
{
    for (size_t i = 0; i < toDoList.size(); ++i)
        for (size_t j = 0; j < doneList.size(); ++j)
            if (toDoList[i].first == doneList[j].first)
                toDoList[i].second -= doneList[j].second;
}