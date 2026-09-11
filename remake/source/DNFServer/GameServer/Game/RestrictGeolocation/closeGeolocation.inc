void RestrictGeolocation::closeGeolocation()
{
    // ORIG 0x08170a88：无空指针判定，直接 GeoIP_delete(this->m_geoip)。
    GeoIP_delete(m_geoip);
}