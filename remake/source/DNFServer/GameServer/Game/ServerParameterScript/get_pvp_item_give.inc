int ServerParameterScript::get_pvp_item_give(int idx)
{
    int ret = -1;
    for (std::vector<int>::const_iterator it = m_vec49dc.begin();
         it != m_vec49dc.end(); ++it)
    {
        if (*it > idx)
        {
            return ret;
        }
        ret = *it;
    }
    return ret;
}