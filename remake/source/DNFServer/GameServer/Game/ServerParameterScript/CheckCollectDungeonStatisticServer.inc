bool ServerParameterScript::CheckCollectDungeonStatisticServer(int serverGroup,
                                                               int channelNo)
{
    bool result = false;
    stServerIndex key;
    key.m_field0 = serverGroup;
    key.m_field4 = channelNo;
    std::map<stServerIndex, bool>::iterator it = m_map0.find(key);
    if (it != m_map0.end())
    {
        result = it->second;
    }
    return result;
