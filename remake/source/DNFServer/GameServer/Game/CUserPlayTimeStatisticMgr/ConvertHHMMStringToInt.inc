// ============================================================================
// ConvertHHMMStringToInt —— ORIG T 0x088c2748
// 形参按值 string；长度 != 5 返回 0；否则 h=atoi(substr(0,2))、
// m=atoi(substr(3,2))，返回 1。
// ============================================================================
int ConvertHHMMStringToInt(std::string s, unsigned char& h, unsigned char& m)
{
    if (s.size() != 5)
    {
        return 0;
    }
    h = static_cast<unsigned char>(atoi(s.substr(0, 2).c_str()));
    m = static_cast<unsigned char>(atoi(s.substr(3, 2).c_str()));
    return 1;
}