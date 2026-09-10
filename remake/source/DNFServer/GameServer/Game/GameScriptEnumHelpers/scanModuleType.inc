int scanModuleType()
{
    std::string token;
    ScanStr(&token);
    return getModuleType(token.c_str());
