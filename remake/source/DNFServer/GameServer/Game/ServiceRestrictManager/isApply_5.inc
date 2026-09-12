bool CreateCharacterRestrictPolicy::isApply(int param)              // ORIG 0816def6
{
    if (param > 2 || param < 0)
    {
        // [R11fixE5] ORIG 0x0816df17 串=0x8b81bd8（raw 逐字节
        // '[Taiwan, Restrict] Invalid AuthMobileRestrictPolicy Parameter. End:%d, Param:%d'
        // —— 原码照抄了 AuthMobile 版文案，照抄）。
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x11c,
                              "[Taiwan, Restrict] Invalid AuthMobileRestrictPolicy Parameter. End:%d, Param:%d",
                              3, param);
        return false;
    }
    return m_svc[param] == 1;
}