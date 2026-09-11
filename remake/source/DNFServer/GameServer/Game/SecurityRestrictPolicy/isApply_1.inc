// ORIG 0x0816db72：param ∈ [0,1] 时返回 m_svc[param] == 1；越界记日志返 false。
bool SecurityRestrictPolicy::isApply(int param)
{
    if (param > 1 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0xcd,
                              "[Taiwan, Restrict] Invalid SecurityRestrictPolicy Parameter. End:%d, Param:%d",
                              2, param);
        return false;
    }
    return m_svc[param] == 1;
}