char eventReward::getSubKeyType(std::string& token)
{
    if (token == "common")  return 0;
    if (token == "level")   return 1;
    if (token == "fatigue") return 2;
    return 0;
