void CUser::add_matched_list(std::vector<std::pair<int, int> >& list, int key, int addValue)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i].first == key)
        {
            list[i].second += addValue;
            return;
        }
    }
    list.push_back(std::make_pair(key, addValue));
