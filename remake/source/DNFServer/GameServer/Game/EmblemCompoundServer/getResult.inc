// getResult —— ORIG T 0x88b1a14：同上形态，命中返回 Result*，未命中 0。
EmblemCompound::Result* EmblemCompound::getResult(int grade)
{
    std::map<int, Result*>::iterator it = m_result.find(grade);
    if (it != m_result.end())
        return it->second;
    return 0;
}