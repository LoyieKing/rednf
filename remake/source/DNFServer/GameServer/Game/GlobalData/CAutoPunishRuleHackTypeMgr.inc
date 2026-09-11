WongWork::CAutoPunishRuleHackTypeMgr::CAutoPunishRuleHackTypeMgr()
{
    // ORIG 0x082a5b1a：构造 4 个容器（multimap<ENUM_HACKTYPE,HackTypeInfo_t>
    // / multimap<u16,HackIPCounter_t> / multimap<u16,HackFullIPCounter_t>
    // / map<int,stHacktypeExceptionInfo_t>），随后向 m_exceptionMap 插入 18 个
    // 异常类型条目（key, {m_cnt, m_flag}）。+0x18 的 12 字节未初始化。
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x83), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xca), stHacktypeExceptionInfo_t(0x28, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xcd), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xce), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd0), stHacktypeExceptionInfo_t(0xc8, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd1), stHacktypeExceptionInfo_t(0x96, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd3), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd7), stHacktypeExceptionInfo_t(0x32, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd8), stHacktypeExceptionInfo_t(0xc8, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x131), stHacktypeExceptionInfo_t(0x1e, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x193), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19b), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19c), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x340), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xdc), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x86), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe0), stHacktypeExceptionInfo_t(0x00, 0x14)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe1), stHacktypeExceptionInfo_t(0x00, 0x32)));
}