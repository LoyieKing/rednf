// getCost —— ORIG T 0x088b19b0：find→end→operator!=，命中返回 value，未命中 -1。
int EmblemCompound::getCost(int grade)
{
    std::map<int, int>::iterator it = m_cost.find(grade);
    if (it != m_cost.end())
        return it->second;
    return -1;
}