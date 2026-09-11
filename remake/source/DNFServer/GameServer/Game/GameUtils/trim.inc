// ---- trim(std::string&)（ORIG _Z4trimRSs @0x0829f068：字符集仅空格）----
void trim(std::string& s)
{
    // P0：ORIG 两调用结果复用同一槽 -0xc ⇒ 原码单变量赋值两次。
    size_t pos = s.find_first_not_of(" ", 0);
    if (pos != std::string::npos)
        s.erase(0, pos);
    pos = s.find_last_not_of(" ", std::string::npos);
    if (pos != std::string::npos)
        s.erase(pos + 1, std::string::npos);
}