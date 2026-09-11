// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus —— ORIG T 0x088e0f58
// m_expBonus(+0x18)[level] 命中 → fildl 值后 flds _DAT_08e0c554(=0.1f，
// rodata 8e0c554: 3d cc cc cd) fdivrp → 值/0.1f；未命中 fldz。
// （GetGoldBonus 同构，map 为 m_goldBonus +0x30。）
// ---------------------------------------------------------------------------
float CharacManageScript::GetExpBonus(int level)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        return static_cast<float>(it->second) / 0.1f;
    return 0.0f;
}