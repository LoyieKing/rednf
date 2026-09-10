STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, float value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = true;   // +0x04 = 1
    m_fValue = value;       // +0x08（浮点值）
    m_nValue = 0;           // +0x0c 清零
    m_nRate = rate;         // +0x10
