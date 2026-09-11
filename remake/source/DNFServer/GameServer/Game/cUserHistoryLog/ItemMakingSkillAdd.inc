// ORIG _ZN15cUserHistoryLog18ItemMakingSkillAddE21ENUM_ITEM_MAKING_TYPEiis @0x08686a88
void cUserHistoryLog::ItemMakingSkillAdd(ENUM_ITEM_MAKING_TYPE type, int a, int b, short c)
{
    m_user->LogHistory("ItemMakingSkill+,%d,%d,%d,%d",
                       (int)type, a, b, (int)c);
}