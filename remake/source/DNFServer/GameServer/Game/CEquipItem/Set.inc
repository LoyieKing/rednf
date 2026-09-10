void RandomItemTable::Set(std::vector<int>* source)
{
    m_vec4.clear();
    m_field0 = 0;
    std::vector<int>::iterator it = source->begin();
    while (it != source->end())
    {
        std::pair<int, int> pr;
        pr.first = *it;
        it++;
        if (it == source->end())
            break;
        pr.second = *it;
        m_field0 += pr.second;
        pr.second = m_field0;
        m_vec4.push_back(pr);
        it++;
    }
