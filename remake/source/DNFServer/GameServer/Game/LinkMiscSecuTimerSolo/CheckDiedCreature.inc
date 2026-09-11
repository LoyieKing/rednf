// ===========================================================================
// user_creature::CCreatureMgr::CheckDiedCreature  ORIG T 0x08339acc：
// m_equippedCreature 非空且其 CheckDiedCreature(m_user) 为真 → true。
// 嵌套 if 形（非 && 短路）：4.4.7 对 `if (a && b) return true;` 会追加 bool
// 物化尾块（+5），ORIG 21 条为统一 leave 双失败出口，嵌套写形与其逐条一致。
// ===========================================================================
bool user_creature::CCreatureMgr::CheckDiedCreature()
{
    if (m_equippedCreature != 0)
        if (m_equippedCreature->CheckDiedCreature(m_user))
            return true;
    return false;
}