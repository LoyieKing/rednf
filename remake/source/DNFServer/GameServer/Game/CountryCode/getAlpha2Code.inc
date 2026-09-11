// ---------------------------------------------------------------------------
// 查询
// ---------------------------------------------------------------------------
std::string CountryCode::getAlpha2Code() const
{
    char t[3] = { 0 };
    t[0] = ccTable[m_idx].alpha2_1;
    t[1] = ccTable[m_idx].alpha2_2;
    return std::string(t);
}