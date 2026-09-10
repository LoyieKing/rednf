short CUser::get_server_fatigue_count(unsigned char type)
{
    // ORIG 0x868d454 call _Rb_tree_iterator::ne + je 尾岛（技巧 30：命中 fall 在前）。
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(type);
    if (it != m_map8eb50.end())
        return it->second;
    return 0;
