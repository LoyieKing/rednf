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
