float CharacManageScript::GetExpBonus(int level)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
