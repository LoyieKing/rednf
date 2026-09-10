void CreatureScriptSkillInfo::clear()
{
    // ORIG 0x833dff0：m_fieldc=1、m_field10=-1，string 从 +0x14 起 5 个
    m_str0.clear();
    m_str4.clear();
    m_field8 = 0;
    m_fieldc = 1;
    m_field10 = -1;
    m_str14.clear();
    m_str18.clear();
    m_str1c.clear();
    m_str20.clear();
    m_str24.clear();
