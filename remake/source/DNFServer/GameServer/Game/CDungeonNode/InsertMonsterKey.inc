// ============================================================================
// CDungeonNode::InsertMonsterKey(int) —— ORIG T 0x084852de
// 已存在返回 false；否则构造 Monster 插入，返回 true。
// ============================================================================
bool CDungeonNode::InsertMonsterKey(int monsterIndex)
{
    // [b28d-solo-CDungeonNode · P293] ORIG 0x084852de：end sret -0x44 先、find sret -0x40 后
    // ⇒ 原码是单表达式 `find(x) != end()`（4.4 单表达式形复现 ORIG end 先/find 后，
    // 具名 it/e 形恒反序）。make_pair 第二参为右值 `Monster&&` ⇒ 传 `Monster()` 临时。
    if (m_monsterMap.find(monsterIndex) != m_monsterMap.end())
        return false;
    m_monsterMap.insert(std::make_pair(monsterIndex, Monster()));
    return true;
}