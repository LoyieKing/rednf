int getObjectTypeToEnumValue(std::string& s)
{
    if (s == "passive")   return 33;
    if (s == "active")    return 17;
    if (s == "monster")   return 529;
    if (s == "character") return 273;
    return -1;
