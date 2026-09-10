void Manager::GetItemListForPacket(
    std::vector<unsigned int, std::allocator<unsigned int> >& out,
    std::map<unsigned int, paramDaily*,
             std::less<unsigned int>,
             std::allocator<std::pair<unsigned int const, paramDaily*> > >& m)
{
    std::map<unsigned int, paramDaily*>::iterator it = m.begin();
    std::map<unsigned int, paramDaily*>::iterator end = m.end();
    while (it != end) {
        InfoDaily* info = m_loader.GetRestrictInfo(it->first);
        if (info != 0) {
            if ((unsigned int)info->m_field8 <= (unsigned int)it->second->m_count)
                out.push_back(it->first);
        }
        it++;
    }
