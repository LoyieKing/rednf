void RestrictGeolocation::getCountryCode(char* out, unsigned int outLen, std::string ip)
{
    if (m_geoip == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x7e, 5);
        trace("[Taiwan, GeoIP] Geo instance is null.");
    }
    std::memset(out, 0, outLen);
    if (GeoIP_country_code_by_addr(m_geoip, ip.c_str()) != 0)
    {
        strcpy(out, GeoIP_country_code_by_addr(m_geoip, ip.c_str()));
    }
}