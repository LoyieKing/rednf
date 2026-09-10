int getCategoryToEnumValue(std::string& s)
{
    if (s == "human")   return 0;
    if (s == "beast")   return 1;
    if (s == "plant")   return 2;
    if (s == "insect")  return 3;
    if (s == "machine") return 4;
    if (s == "hybrid")  return 6;
    if (s == "undead")  return 7;
    if (s == "devil")   return 8;
    if (s == "spirit")  return 9;
    if (s == "dragon")  return 10;
    if (s == "angel")   return 11;
    return 44; // ORIG 默认分支 mov $0x2c,%eax
