bool RestrictGeolocation::initGeolocation()
{
    // ORIG 0x08170a46：m_geoip 为空则 GeoIP_new(0)；失败返回 0；随后 loadData()。
    if (m_geoip == 0)
    {
        m_geoip = GeoIP_new(0);
        if (m_geoip == 0)
        {
            return false;
        }
    }
    loadData();
    return true;
}