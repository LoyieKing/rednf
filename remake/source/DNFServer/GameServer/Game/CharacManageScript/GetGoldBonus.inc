// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus —— ORIG T 0x088e0fce（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
float CharacManageScript::GetGoldBonus(int level)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}