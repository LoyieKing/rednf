IBuyRule* CSecretShop::GetRule(eBuyRule rule)
{
    // ORIG：`if (it != end) return it->second; return 0;`（ne 比较，非 eq+取反）。
    std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.find(rule);
    if (it != m_rules.end())
        return it->second;
    return 0;
}