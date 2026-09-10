int EmblemCompound::getCost(int grade)
{
    std::map<int, int>::iterator it = m_cost.find(grade);
    if (it != m_cost.end())
        return it->second;
    return -1;
