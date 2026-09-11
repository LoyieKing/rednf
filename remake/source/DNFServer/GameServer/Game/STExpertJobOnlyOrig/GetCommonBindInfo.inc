// ----------------------------------------------------------------------------
// GetCommonBindInfo — ORIG W 0x0822b366 range [...b374]：return this+0x84
// GetSpecialBindInfo — ORIG W 0x0822b358 range [...b366]：return this+0xc4
// ----------------------------------------------------------------------------
STExpertJobScript::stMonsterCardBindInfo* STExpertJobScript::GetCommonBindInfo()
{
    return &m_commonBindInfo;   // this+0x84
}