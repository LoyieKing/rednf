// ORIG C1 0x837cf46：仅构造 +0x4/+0x10/+0x1c/+0x28 四 vector，+0x00 不初始化。
stCharacLinkSystem_t::stCharacLinkSystem_t()
    : m_vec4(), m_vec10(), m_vec1c(), m_vec28()
{
}