// ---------------------------------------------------------------------------
// CharacManageScript::GetOptionValue —— ORIG T 0x088e11fc
// m_manageOption[type] 命中 → second，未命中 → 0。
// ---------------------------------------------------------------------------
int CharacManageScript::GetOptionValue(int type)
{
    std::map<int, int>::iterator it = m_manageOption.find(type);
    if (it != m_manageOption.end())
        return it->second;
    return 0;
}