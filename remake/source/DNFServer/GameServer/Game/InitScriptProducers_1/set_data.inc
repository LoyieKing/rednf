void stStepRewardStackableItemScript::set_data(const int step, StackableItemData& data)
{
    std::map<int, std::vector<StackableItemData> >::iterator it = m_map0.find(step);
    if (m_map0.end() == it)
    {
        std::vector<StackableItemData> items;
        items.push_back(data);
        m_map0.insert(std::make_pair(step, items));
    }
    else
    {
        it->second.push_back(data);
    }
}
