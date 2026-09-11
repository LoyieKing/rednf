void CCompoundAvatar::tagCompoundInfo::clear()
{
    m_field00     = 0xb;      // 表索引复位（0xb == 表元素数）
    m_field04     = 0;
    m_field08     = 0;
    m_materialA.reset();
    m_field48     = 0;
    m_materialB.reset();
    m_scriptA     = 0;
    m_scriptB     = 0;
    m_field90     = 0;
    m_avatar.reset();
    m_fieldD0     = 0;
    m_slot        = 100;      // 0x64
    m_errorFlag   = 0;
    m_fieldD6     = 0;
    m_type        = 0;
    m_resultSlot  = 0xffff;
    m_results.clear();
}