// ----------------------------------------------------------------------------
// ConvertHHMMSSStringToInt —— ORIG T 0x088c25df。"HH:MM:SS"（8 字符）：
// h=atoi(substr(0,2))、m=atoi(substr(3,2))、s=atoi(substr(6,2))。
// 长度非 8 返回 false。
// ----------------------------------------------------------------------------
bool ConvertHHMMSSStringToInt(std::string s, unsigned char& h,
                              unsigned char& m, unsigned char& sec)
{
    if (s.size() != 8)
    {
        return false;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    sec = static_cast<unsigned char>(atoi(s.substr(6, 2).c_str()));
    return true;
}