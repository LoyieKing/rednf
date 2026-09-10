bool AuthMobileRestrictPolicy::beginPolicy(std::string const& str)
{
    // ORIG 读 this+0x8（m_code）传 setPolicyApplyValue。
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
