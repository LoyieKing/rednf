EmblemCompound::Result* EmblemCompound::getResult(int grade)
{
    std::map<int, Result*>::iterator it = m_result.find(grade);
    if (it != m_result.end())
        return it->second;
    return 0;
