bool ServerParameterScript::isDungeonOpen(int dungeonIdx)
{
    std::vector<int>::iterator it =
        std::find(m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
    // ORIG 0x085bf0xx 尾部实证 test/je + 两分支均 mov$1：原源码即
    // 「found 与否都 return true」的死分支形（忠实还原）。
    if (it != m_dungeonOpenE0.end())
    {
        return true;
    }
    return true;
