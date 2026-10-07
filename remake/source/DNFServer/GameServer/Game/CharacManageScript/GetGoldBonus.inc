// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus —— ORIG T 0x088e0fce（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
// [bughunt13] ORIG GetGoldBonus 常量同为 100.0f（fdivrp ⇒ 值/100）；旧码 /0.1f 误。
float CharacManageScript::GetGoldBonus(int level)
{
    std::map<int, int>::iterator it = m_goldBonus.find(level);
    if (it != m_goldBonus.end())
        return static_cast<float>(it->second) / 100.0f;
    return 0.0f;
}