bool isValidExtension(const std::string& a, const std::string& b)
{
    // ORIG 0x8a3c6b7 `cmp -0xc(%ebp),%eax; jge`（有符号序）⇒ 长度局部为
    // signed int（size_t 局部 4.4 恒 jae 无符号序，P196 反演）。
    int n1 = (int)a.length();
    int n2 = (int)b.length();
    if (n1 == 0)
        return true;
    if (n1 < n2)
        return false;
    std::string s = a.substr(n1 - n2, n2);
    return s.compare(b) == 0;
