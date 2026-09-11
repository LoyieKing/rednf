bool AuthMobileRestrictPolicy::isApply(int param)                   // ORIG 0816d7f0
{
    if (param > 0x21 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x84,
                              "[Taiwan, Restrict] Invalid AuthMobileRestrictPolicy Parameter. End:%d, Param:%d",
                              0x22, param);
        return false;
    }
    return m_svc[param] == 1;
}