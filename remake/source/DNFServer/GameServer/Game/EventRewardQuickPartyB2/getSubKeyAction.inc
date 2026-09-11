// ORIG T 0x080ef128：[/sub key] 行名 → 发放动作（equip=2 / inven=1 / postal=3）。
char eventReward::getSubKeyAction(std::string& token)
{
    if (token == "equip")  return 2;
    if (token == "inven")  return 1;
    if (token == "postal") return 3;
    return 0;
}