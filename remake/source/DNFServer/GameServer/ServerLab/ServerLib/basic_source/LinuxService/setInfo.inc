void nsl::ServiceInfo::setInfo(const char* p_szServiceName, const char* p_szDisplayName,
                               const char* p_szBinaryName, const char* p_szConfigFileName)
{
    puts("In ServiceInfo::setInfo");
    if (p_szServiceName != NULL)
    {
        strncpy(m_szServiceName, p_szServiceName, 0x100);
    }
    else
    {
        m_szServiceName[0] = '\0';
    }
    if (p_szDisplayName != NULL)
    {
        strncpy(m_szDisplayName, p_szDisplayName, 0x100);
    }
    else
    {
        m_szDisplayName[0] = '\0';
    }
    if (p_szBinaryName != NULL)
    {
        strncpy(m_szBinaryName, p_szBinaryName, 0x100);
    }
    else
    {
        m_szBinaryName[0] = '\0';
    }
    if (p_szConfigFileName != NULL)
    {
        strncpy(m_szConfigFileName, p_szConfigFileName, 0x100);
        setPIDFileName(m_szConfigFileName);
    }
    else
    {
        m_szConfigFileName[0] = '\0';
    }
    puts("Out ServiceInfo::setInfo");
