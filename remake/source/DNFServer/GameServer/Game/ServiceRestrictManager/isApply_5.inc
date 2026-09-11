bool CreateCharacterRestrictPolicy::isApply(int param)              // ORIG 0816def6
{
    if (param > 2 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x11c,
                              "[Taiwan, Restrict] Invalid CreateCharacterRestrictPolicy Parameter. End:%d, Param:%d",
                              3, param);
        return false;
    }
    return m_svc[param] == 1;
}