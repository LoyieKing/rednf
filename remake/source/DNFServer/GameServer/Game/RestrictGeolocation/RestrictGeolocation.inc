RestrictGeolocation::RestrictGeolocation() : m_geoip(0), m_allowCountry(0xb)
{
    // ORIG 0x0817097c：编译器先调基类 ARAD::Singleton<RestrictGeolocation>::Singleton()
    // （报告 call 081713a6）→ m_geoip=0 → unordered_set 以 hash=0xb 构造
    // （报告 call 08171406，四参：this+4, 0xb, &equal_to, &allocator）。体为空。
