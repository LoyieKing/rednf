bool ServerParameterScript::CheckLightServer(int serverIdx)
{
    bool result = false;
    std::map<int, bool>::iterator it = m_map24.find(serverIdx);
    if (it != m_map24.end())
    {
        result = it->second;
        SetLightServerFlag(result);
    }
    return result;
