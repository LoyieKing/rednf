// ---------------------------------------------------------------------------
// hasNPC：m_npcIdList 是否包含指定 NPC
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 8=8；46=46）。修法：具名 `const_iterator it =
// std::find(...)` 后再 `it == end()`——P293 求值序旋钮：ORIG 086c214c eq(find(-0x18),
// end(-0xc)) 要求 find 先求值；单表达式 `find()==end()` 被 4.4 反序（先求右 end()），
// `end()==find()` 则 eq 实参序反。具名形两侧逐条全等。
bool Area::hasNPC(unsigned int npcId) const
{
    // [b28d-solo-Area] ORIG 086c214c 调 `_ZN9__gnu_cxxeq…` 后 test/je 两臂
    // （P56：bool 返回勿直 return 表达式；P196：eq+取反=源形 `==` 加分支）。
    std::vector<int>::const_iterator it =
        std::find(m_npcIdList.begin(), m_npcIdList.end(), npcId);
    if (it == m_npcIdList.end())
        return false;
    return true;
}