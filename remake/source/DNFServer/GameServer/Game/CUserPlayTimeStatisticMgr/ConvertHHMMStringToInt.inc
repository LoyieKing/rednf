int ConvertHHMMStringToInt(std::string s, unsigned char& h, unsigned char& m)
{
    if (s.size() != 5)
    {
        return 0;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    return 1;
