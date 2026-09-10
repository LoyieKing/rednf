int Action::run(int type, input_param& p)
{
    std::map<int, boost::function<int(input_param&)> >::iterator it = m_map2.find(type);
    if (it != m_map2.end())
    {
        return it->second(p);
    }
    return 0;
