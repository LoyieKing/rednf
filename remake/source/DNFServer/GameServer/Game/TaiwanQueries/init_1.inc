bool AuthGarenaToken::init(const char* path)
{
    // ORIG 81730d6：arg==empty（0x8b83f20）走带参 Init(path)，
    // 否则无参 Init()（= Init("publickey.pem"@0x8b81361)）。
    std::string empty("");
    // [b28d-solo-D02] ORIG 0x08173115 直接压 path 形参（无判空三目，P171）。
    std::string arg(path);
    if (arg == empty)
        return openSSLRSA_.Init(path);
    return openSSLRSA_.Init();
