void nsl::ServiceInfo::setPIDFileName(const char* p_szPIDFileName)
{
    puts("In ServiceInfo::setPIDFileName");
    std::string pidFileName(p_szPIDFileName);
    int size = pidFileName.find_last_of(".") - pidFileName.find_last_of("/") - 1;
    pidFileName = pidFileName.substr(pidFileName.find_last_of("/") + 1, size);
    if (p_szPIDFileName != NULL)
    {
        strncpy(m_szPIDFileName, pidFileName.c_str(), 0x100);
    }
    else
    {
        m_szPIDFileName[0] = '\0';
    }
    puts("Out ServiceInfo::setPIDFileName");
