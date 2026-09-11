// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus(int, int*) —— ORIG T 0x088e1044
// 命中 m_expBonus[level] 写 *out，未命中不动 *out（void 返回）。
// ---------------------------------------------------------------------------
void CharacManageScript::GetExpBonus(int level, int* out)
{
    std::map<int, int>::iterator it = m_expBonus.find(level);
    if (it != m_expBonus.end())
        *out = it->second;
}