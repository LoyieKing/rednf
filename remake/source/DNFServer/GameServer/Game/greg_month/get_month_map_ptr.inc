greg_month::month_map_ptr_type greg_month::get_month_map_ptr()
{
    static month_map_ptr_type month_map_ptr(new month_map_type);
    // 2026-08-31：ORIG 0x87dcdfa 额外 new 16B 辅助对象（存全局 0x9491d4c，
    // vtable 0x8de0de0；本实现建表不用它——纯分配行为对齐）。
    static void* auxMonthObj = ::operator new(16);
    static bool initialized = false;
    (void)auxMonthObj;
    if (!initialized)
    {
        for (unsigned short i = 1; i <= 12; ++i)
        {
            std::string s(kLongMonthNames[i - 1]);
            std::string abbr(kShortMonthNames[i - 1]);
            for (std::string::iterator it = s.begin(); it != s.end(); ++it)
                *it = static_cast<char>(std::tolower(static_cast<unsigned char>(*it)));
            for (std::string::iterator it = abbr.begin(); it != abbr.end(); ++it)
                *it = static_cast<char>(std::tolower(static_cast<unsigned char>(*it)));
            (*month_map_ptr)[s] = i;
            (*month_map_ptr)[abbr] = i;
        }
        initialized = true;
    }
    return month_map_ptr;
}