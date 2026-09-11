unsigned char const* CountryCode::getAlpha2CodeByte() const
{
    return &ccTable[m_idx].alpha2_1;
}