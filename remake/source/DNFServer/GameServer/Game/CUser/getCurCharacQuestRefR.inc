// ORIG W 0x082604ec _ZNK5CUser21getCurCharacQuestRefR Ev（const）
// 返回 m_quest（+0x858d4）引用指针；与 getCurCharacQuestR 同址、独立符号。
UserQuest* CUser::getCurCharacQuestRefR() const
{
    return const_cast<UserQuest*>(&m_quest);
}