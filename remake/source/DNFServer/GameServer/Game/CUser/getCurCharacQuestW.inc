// ORIG W 0x0814aa5e：enableSaveQuest() 后返回 m_quest（+0x858d4）。
UserQuest* CUser::getCurCharacQuestW()
{
    enableSaveQuest();
    return (UserQuest*)&m_quest;
}