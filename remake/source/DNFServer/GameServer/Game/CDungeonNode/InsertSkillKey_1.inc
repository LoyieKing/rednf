// ============================================================================
// CDungeonJobNode::InsertSkillKey(int) —— ORIG T 0x08484a86
// 已存在返回 false；否则构造 Skill 插入，返回 true。
// ============================================================================
bool CDungeonJobNode::InsertSkillKey(int skillIndex)
{
    // [b28d-solo-CDungeonNode · P293] ORIG 0x08484a86：end sret -0x50 先、find sret -0x4c 后
    // ⇒ 单表达式 `find(x) != end()`（具名 it/e 形 4.4 恒反序）。
    // make_pair 第二参为右值 `Skill&&` ⇒ 源形传 `Skill()` 临时。
    if (m_skillMap.find(skillIndex) != m_skillMap.end())
        return false;
    m_skillMap.insert(std::make_pair(skillIndex, Skill()));
    return true;
}