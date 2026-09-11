std::string CountryCode::getCountryName() const
{
    return std::string(toTString(ccTable[m_idx].name));
}