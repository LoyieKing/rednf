int GameWorld::getDungeonMinimumRequiredLevel(int dungeonIdx)
{
    CDungeon* dungeon = (CDungeon*)G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == NULL)
    {
        return -1;
    }
    return dungeon->get_min_level();
