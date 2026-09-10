float CharacManageScript::GetGoldBonus(int level)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
