void RecipeInfo::changeOutput(std::vector<std::pair<int, int> >& out)
{   // ORIG W 0x08693a78：out.clear(); out = m_vec40（this+0x40 向量赋值）
    out.clear();
    out = m_vec40;
