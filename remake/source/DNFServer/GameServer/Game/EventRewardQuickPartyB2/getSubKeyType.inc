// ORIG T 0x080ef0c0：[sub type] 行名 → 子键类型。
char eventReward::getSubKeyType(std::string& token)
{
    if (token == "common")  return 0;
    if (token == "level")   return 1;
    if (token == "fatigue") return 2;
    return 0;
}