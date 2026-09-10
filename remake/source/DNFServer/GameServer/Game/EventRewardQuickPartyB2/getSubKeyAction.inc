char eventReward::getSubKeyAction(std::string& token)
{
    if (token == "equip")  return 2;
    if (token == "inven")  return 1;
    if (token == "postal") return 3;
    return 0;
