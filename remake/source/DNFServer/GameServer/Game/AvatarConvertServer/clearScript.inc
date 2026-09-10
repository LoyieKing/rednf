void AvatarConvert::clearScript()
{
    std::map<int, ConvertResult*>::iterator it = m_convertMap.begin();
    std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
    for (; it != endIt; ++it)
        delete it->second;
    m_convertMap.clear();
