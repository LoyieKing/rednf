bool WarRoom::IsBanUser(CUser* user)
{
    unsigned int id = user->get_acc_id();
    std::map<unsigned int, int>::iterator it = m_map.find(id);
    // ORIG 0x86ba9e7：end() 临时量内联于比较表达式（slot 布局 it=-0x14/临时=-0xc 实证）；
    // bool 返回值在 if 中单层否定 → 具名 0/1 返回。
    if (it != m_map.end())
        return 1;
    return 0;
