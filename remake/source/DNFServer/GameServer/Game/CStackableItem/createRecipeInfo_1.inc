void createRecipeInfo(RecipeInfo* info, std::vector<int>* source,
                      std::vector<std::pair<int, int> >* extra)
{
    info->clear();
    // it/end 槽方向：本波 **P326** 归零（三形读数并列见本函数头判词）。
    std::vector<int>::iterator it = source->begin();
    if (it == source->end())
    {
        return;
    }
    // [b28d-solo-CStackableItem] ORIG 三个 count 局部各占独立槽
    // （0x89ba93e `mov %eax,-0x18`、第二段 `-0x10`、第三段 `-0xc`）⇒ 源码为
    // 三个不同具名局部（复用同一变量时 4.4/4.1 都只给一个槽）。
    int n1 = *it;
    it++;
    int i = 0;
    for (i = 0; i < n1; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec0.push_back(p);
    }
    int n2 = *it;
    it++;
    for (i = 0; i < n2; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vecC.push_back(p);
    }
    int n3 = *it;
    it++;
    for (i = 0; i < n3; ++i)
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        p.second = *it;
        it++;
        info->m_vec18.push_back(p);
    }
    if (it == source->end())
    {
        return;
    }
    info->m_field24 = (*it == 1);
    info->m_field25 = (*it == 2);
    if (extra != 0)
    {
        info->m_vec28 = *extra;
    }
