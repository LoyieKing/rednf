void CharacManageScript::GetGoldBonus(int level, int* out)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        *out = it->second;
