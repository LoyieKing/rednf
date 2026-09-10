int scanWorldType()
{
    std::string token;
    ScanStr(&token);
    return getWorldType(token.c_str());
