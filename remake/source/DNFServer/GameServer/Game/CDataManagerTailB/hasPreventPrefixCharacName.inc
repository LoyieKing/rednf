int CDataManager::hasPreventPrefixCharacName(const char* name, int /*unused*/)
{
    for (int i = 0; m_serverParameter.m_preventPrefixCount > i; ++i)
    {
        const size_t entryLen =
            strlen(&m_serverParameter.m_preventPrefixEntries[i * 0x1e]);
        char prefix[0x15] = { '\0' };
        memcpy(prefix, name, entryLen);
        if (strcmp(&m_serverParameter.m_preventPrefixEntries[i * 0x1e],
                   prefix) == 0)
            return 1;
    }
    return 0;
