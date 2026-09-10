int Action::check(int type, input_param& p)
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map1.find(type);
    if (it != m_map1.end())
    {
        return it->second(p);
    }
    return 0;
