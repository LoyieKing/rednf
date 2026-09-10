std::multimap<int, int>* QuestList::getQuestNPCList(ENUM_QUEST_GRADE grade)
{
    switch (grade)
    {
    case ENUM_QUEST_GRADE_NONE:
        return &m_npcQuestList0;
    case ENUM_QUEST_GRADE_1:
        return &m_npcQuestList3;
    case ENUM_QUEST_GRADE_2:
        return &m_npcQuestList4;
    case ENUM_QUEST_GRADE_4:
        return &m_npcQuestList2;
    case ENUM_QUEST_GRADE_5:
        return &m_npcQuestList1;
    case ENUM_QUEST_GRADE_6:
        return &m_npcQuestList5;
    case ENUM_QUEST_GRADE_8:
        return &m_npcQuestList6;
    default:
        return 0;
    }
