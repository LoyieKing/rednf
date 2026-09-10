int getNpcMoodFromString(const char* name)
{
    if (std::strncmp("good", name, 10) == 0)   return 2;
    if (std::strncmp("normal", name, 10) == 0) return 1;
    if (std::strncmp("bad", name, 10) == 0)    return 0;
    return 3;
