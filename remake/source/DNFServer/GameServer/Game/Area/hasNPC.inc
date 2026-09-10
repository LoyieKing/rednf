bool Area::hasNPC(unsigned int npcId) const
{
    // [b28d-solo-Area] ORIG 086c214c 调 `_ZN9__gnu_cxxeq…` 后 test/je 两臂
    // （P56：bool 返回勿直 return 表达式；P196：eq+取反=源形 `==` 加分支）。
    std::vector<int>::const_iterator it =
        std::find(m_npcIdList.begin(), m_npcIdList.end(), npcId);
    if (it == m_npcIdList.end())
        return false;
    return true;
