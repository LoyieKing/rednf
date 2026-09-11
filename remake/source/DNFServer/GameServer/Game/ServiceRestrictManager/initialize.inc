// ORIG 0x0816e2b2：channel_no==1 → 置 +0x19=1；创建 4 个策略对象注册进 map。
bool ServiceRestrictManager::initialize()
{
    // 复用单指针变量（ORIG -0x1c(%ebp) 单槽，函数开头预置 0）。
    InterfaceServicePolicy* p = 0;
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        m_flag19 = 1;
    }
    // ORIG 0x0816e2e2：每策略独立 new（普通 _Znwj 0x18）→ 判空 →
    // 独立 logFormat（行号 0x169/0x171/0x17a/0x183）→ return false。
    p = new NullPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x169,
                              "[Taiwan, Restrict] Fail create NullPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new AuthMobileRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x171,
                              "[Taiwan, Restrict] Fail create AuthMobileRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new CreateCharacterRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x17a,
                              "[Taiwan, Restrict] Fail create CreateCharacterRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new SecurityRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x183,
                              "[Taiwan, Restrict] Fail create SecurityRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    return true;
}