// ============================================================================
// loadValueType —— ORIG 0x08892ac4（_Z13loadValueTypeRKSs）
// "+"=0、"%"=1、其他=2。
// ============================================================================
int loadValueType(const std::string& token)
{
    if (token == "+")
        return 0;
    if (token == "%")
        return 1;
    return 2;
}