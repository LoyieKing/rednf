// ---- ORIG T 0x088c1430 ----
int scanWorldType()
{
    std::string token;
    ScanStr(&token);
    return getWorldType(token.c_str());
}