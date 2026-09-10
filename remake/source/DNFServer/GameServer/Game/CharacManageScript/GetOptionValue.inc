int CharacManageScript::GetOptionValue(int type)
{
    std::map<int, int>::iterator it = m_manageOption.find(type);
    if (it != m_manageOption.end())
        return it->second;
    return 0;
