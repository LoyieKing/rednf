bool parseIntToken(int& value)
{
    std::string token;
    if (!nextToken(token)) return false;
    char* end = 0;
    long parsed = std::strtol(token.c_str(), &end, 0);
    if (end == token.c_str() || *end != '\0') return false;
    value = static_cast<int>(parsed);
    return true;
