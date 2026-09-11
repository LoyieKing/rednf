int NPC_Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x082840f6：r = Teleport::check_rule(data)；r==0 → 返回 0。
    int r = Teleport::check_rule(data);
    if (r != 0)
    {
        return r;
    }
    return 0;
}