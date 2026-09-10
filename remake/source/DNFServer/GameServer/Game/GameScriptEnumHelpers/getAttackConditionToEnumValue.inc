int getAttackConditionToEnumValue(std::string& s)
{
    if (s == "critical")     return 0;
    if (s == "counter")      return 1;
    if (s == "back attack")  return 2;
    if (s == "stuck")        return 3;
    return 4;
