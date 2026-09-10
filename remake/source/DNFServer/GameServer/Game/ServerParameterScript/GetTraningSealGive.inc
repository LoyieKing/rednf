int ServerParameterScript::GetTraningSealGive(int idx) const
{
    if (m_map4994.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map4994.find(idx);
    if (it != m_map4994.end())
    {
        return it->second;
    }
    // ORIG 实证：!empty() 正形(xor$1)，it 复用 = end() 临时+拷贝赋值
    if (!m_map4994.empty())
    {
        it = m_map4994.end();
        --it;
        return it->second;
    }
    return 0;
