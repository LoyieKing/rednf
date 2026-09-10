int Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x08283d82：转发 check_rule(data.m_user)。
    return check_rule(data.m_user);
