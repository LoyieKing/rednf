// ORIG 0x0816db2e：setPolicyApplyValue(m_code, m_svc, str)；返回 1。
bool SecurityRestrictPolicy::beginPolicy(std::string const& str)
{
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}