// ORIG W 0x08985e0a：基类 → string+0x8 → vec<int>+0x10 →
// vector<pair<vector<uint>,mail>>+0x1c → vector<pair<Rule,mail>>+0x28 →
// str.clear → 字节+c=0。
ObjectBringUpScript::ObjectBringUpScript()
{
    m_dbTableName.clear();
    m_exchangeRuleFlag = 0;
}