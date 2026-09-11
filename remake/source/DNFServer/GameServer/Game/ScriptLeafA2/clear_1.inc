// ============================================================================
// stLevelUpRewardItemScript::clear —— ORIG W 0x083711d6
// 【报告缺失】无专属函数报告；已先读调用者报告
// _lt_global_gt_/ImportLevelUpRewardItemScript.md 后按 nm 地址 objdump 转写：
//   83711dc mov 0x8(%ebp),%eax ; push eax
//   83711e2 call std::map<LevelUpRewardItemTableKey,LevelUpRewardItemData>::clear
// 即 m_map0.clear()。
// ============================================================================
void stLevelUpRewardItemScript::clear()
{
    m_map0.clear();
}