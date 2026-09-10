bool ConvertStringToTime(std::string s, long& out)
{
    if (s.size() != 0x13)
    {
        return false;
    }
    struct tm t;
    t.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;  // -0x38（ORIG 首段 sub $0x76c）
    t.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;   // -0x3c
    t.tm_mday = atoi(s.substr(8, 2).c_str());   // -0x40
    t.tm_hour = atoi(s.substr(11, 2).c_str());  // -0x44
    t.tm_min = atoi(s.substr(14, 2).c_str());   // -0x48
    t.tm_sec = atoi(s.substr(17, 2).c_str());   // -0x4c
    // tm_wday/tm_yday/tm_isdst 保持未初始化（与 ORIG 栈槽一致，mktime 归一化）
    out = mktime(&t);
    return true;
