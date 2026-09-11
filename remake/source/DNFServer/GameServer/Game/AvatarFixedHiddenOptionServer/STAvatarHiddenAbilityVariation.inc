// ============================================================================
// STAvatarHiddenAbilityVariation —— ORIG ctor W 0x088b01e8 / 0x088b021a、dtor W 0x088b0aae
// ============================================================================
STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, int value, int rate) throw()
{
    m_type = type;          // +0x00
    m_bFloatValue = false;  // +0x04 = 0
    m_fValue = 0.0f;        // +0x08 清零
    m_nValue = value;       // +0x0c
    m_nRate = rate;         // +0x10
}