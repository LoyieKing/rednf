std::vector<unsigned long>* EmblemCompound::getList(int grade)
{
    // ORIG：先默认构造迭代器，再赋值 find 结果（帧内多出 C1 调用与拷贝）。
    std::map<int, Result*>::iterator it;
    it = m_result.find(grade);
    if (it != m_result.end())
        return &m_list[it->second->m_listIdx];
    return 0;
}