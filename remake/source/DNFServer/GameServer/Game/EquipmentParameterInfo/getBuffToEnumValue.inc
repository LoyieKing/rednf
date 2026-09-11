int getBuffToEnumValue(std::string& s)
{
    if (s == "superarmor")   return 0;
    if (s == "unbreakable")  return 1;
    if (s == "camoflage")    return 2;
    return 3;
}