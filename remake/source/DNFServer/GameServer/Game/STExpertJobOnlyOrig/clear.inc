// ----------------------------------------------------------------------------
// stSelfDisjoinMaterialInfo::clear — ORIG W 0x0896dbe6 range [...dc0c]
//   +8=0；+4=+8；+0=+4（链式传递，全零效果）
// ----------------------------------------------------------------------------
void STExpertJobScript::stSelfDisjoinMaterialInfo::clear()
{
    m_field8 = 0;
    m_field4 = m_field8;
    m_field0 = m_field4;
}