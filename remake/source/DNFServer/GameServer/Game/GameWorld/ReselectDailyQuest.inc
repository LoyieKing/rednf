bool GameWorld::ReselectDailyQuest()
{
    if (!G_CDataManager()->reselectDailyTrainingQuest())
    {
        return false;
    }
    return true;
}