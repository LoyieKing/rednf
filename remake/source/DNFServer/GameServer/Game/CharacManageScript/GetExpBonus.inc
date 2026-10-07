// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus —— ORIG T 0x088e0f58
// m_expBonus(+0x18)[level] 命中 → fildl 值后 flds _DAT_08e0c554(=100.0f，
// rodata 8e0c554: 00 00 c8 42) fdivrp → 值/100.0f；未命中 fldz。
// （GetGoldBonus 同构，map 为 m_goldBonus +0x30。）
// ---------------------------------------------------------------------------
// [bughunt13] objdump -s 0x8e0c554 = 00 00 c8 42 = 100.0f（旧注 3dcccccd/0.1f 误读）⇒ 值/100（旧码 /0.1 = ×1000 倍）。
float CharacManageScript::GetExpBonus(int level)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        return static_cast<float>(it->second) / 100.0f;
    return 0.0f;
}