void CharacManageScript::GetExpBonus(int level, int* out)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        *out = it->second;
