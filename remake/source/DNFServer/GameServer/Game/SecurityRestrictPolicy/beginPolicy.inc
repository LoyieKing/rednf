bool SecurityRestrictPolicy::beginPolicy(std::string const& str)
{
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
