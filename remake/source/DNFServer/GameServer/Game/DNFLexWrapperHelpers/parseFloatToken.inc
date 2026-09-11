bool parseFloatToken(float& value)
{
    std::string token;
    if (!nextToken(token)) return false;
    char* end = 0;
    float parsed = static_cast<float>(std::strtod(token.c_str(), &end));
    if (end == token.c_str() || *end != '\0') return false;
    value = parsed;
    return true;
}