// [b28d-32 N(P223/N89): ORIG 空块岛与迭代器/STL 形态差（81/93） | ORIG 0x08353c86]
void QuestList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x08353c86：for-init 逗号式同时直构 it=begin()
    // 与 end=m.end()（两个 sret 槽 -0x14/-0x1c，ORIG 条件区不重调 end）+
    // operator*（deEv，P167 ⇒ `(*it)`）+ 前置 `++it`（ppEv）+ quest 无 0 初始化。
    Quest* quest;
    for (__gnu_cxx::hash_map<int, Quest*>::iterator it = m_questMap.begin(),
             end = m_questMap.end();
         it != end;
         ++it)
    {
        quest = (*it).second;
        delete quest;
    }
    m_questMap.clear();
    m_npcQuestList0.clear();
    m_npcQuestList1.clear();
    m_npcQuestList2.clear();
    m_npcQuestList7.clear();
    m_npcQuestList3.clear();
    m_npcQuestList4.clear();
    m_npcQuestList5.clear();
    m_listE0.clear();
    m_npcQuestList6.clear();
}