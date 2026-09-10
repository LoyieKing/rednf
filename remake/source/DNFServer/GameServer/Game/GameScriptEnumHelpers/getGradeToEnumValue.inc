int getGradeToEnumValue(std::string& s)
{
    if (s == "common")   return 0;
    if (s == "champion") return 1;
    if (s == "boss")     return 2;
    if (s == "named")    return 3;
    return 4;
