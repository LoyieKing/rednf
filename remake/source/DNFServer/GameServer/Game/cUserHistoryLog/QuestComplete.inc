// ORIG _ZN15cUserHistoryLog13QuestCompleteEi @0x0868514e（格式串 ORIG .rodata 0x8cf0957）
void cUserHistoryLog::QuestComplete(int questIdx)
{
    m_user->LogHistory("QuestComplete,%d", questIdx);
}