// ORIG 0x080e0e68（报告 GetItemListForPacket.md 全文转写）：
//   遍历 m（begin/end 各求值一次，while 循环后缀 ++）：
//   GetRestrictInfo(it->first) 命中且 info->m_field8 <= it->second->m_count
//   （无符号比较 setbe）时 out.push_back(it->first)。
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
}