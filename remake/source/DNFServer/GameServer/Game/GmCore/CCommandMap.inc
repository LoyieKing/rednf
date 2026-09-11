// ---- CCommandMap ----
CCommandMap::CCommandMap()                     // 0x082a7f64
    : m_byId(),
      m_byName(10, std::hash<std::string>(), std::equal_to<std::string>())
{
    // ORIG：unordered_map 以 bucket_count=10 直接构造。
}