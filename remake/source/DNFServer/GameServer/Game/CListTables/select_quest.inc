void QuestList::select_quest(std::list<int>& out, stSelectQuestParam& param,
                             const WongWork::CQuestClear& clear)
{
    out.clear();
    allowable_questlist_as_npc(out, m_npcQuestList0, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList1, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList2, param, clear, true);
    allowable_questlist_as_npc(out, m_npcQuestList7, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList3, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList4, param, clear, false);
    allowable_questlist_as_npc(out, m_npcQuestList5, param, clear, false);
