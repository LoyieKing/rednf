std::string CharacManageScript::GetName(unsigned int key)
{
    std::map<int, std::string>::iterator it = m_basicExplain.find((int)key);
    if (it != m_basicExplain.end())
        return it->second;
    return std::string();
