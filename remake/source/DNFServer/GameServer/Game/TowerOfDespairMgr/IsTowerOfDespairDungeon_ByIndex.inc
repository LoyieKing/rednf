// TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex —— ORIG T 0x08644272
// [0x08644272,0x086442c9]：m_layerSet(+0x328, set<int>).find(idx) != end()。
// const 成员：find/end 返回 _Rb_tree_const_iterator（与报告一致），
// operator!= 结果直返（无规范化，返回类型 bool）。
// 注意：find 必须独立语句先行——单语句 `find(idx) != end()` 被 gcc
// 从右向左求值（先 end 后 find），与 ORIG 指令序相反；
// 且 ORIG operator!= 的 this=end()、实参=it（写 `end() != it`）。
bool TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(int idx) const
{
    std::set<int>::const_iterator it = m_layerSet.find(idx);
    return m_layerSet.end() != it;
}