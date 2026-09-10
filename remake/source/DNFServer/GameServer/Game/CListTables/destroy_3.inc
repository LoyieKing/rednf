void CSkillList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x08350b42：每轮取具名指针 p=&m_skillList[i]
    // （-0x14，地址计算 i*0x14+this 存槽）+ q=&m_groupSkillMap[i]（-0xc，
    // i*0x18+0xdc+this）+ 内层 for-init 逗号式直构 begin/end（无 C1Ev）+
    // `(*it)`（deEv）/前置 `++it`（ppEv）；skill 无 0 初始化。
    for (int i = 0; i <= 10; ++i)
    {
        __gnu_cxx::hash_map<int, CSkill*>* p = &m_skillList[i];
        CSkill* skill;
        for (__gnu_cxx::hash_map<int, CSkill*>::iterator it = p->begin(),
                 end = p->end();
             it != end;
             ++it)
        {
            skill = (*it).second;
            delete skill;
        }
        p->clear();
        std::multimap<int, CSkill*>* q = &m_groupSkillMap[i];
        q->clear();
    }
