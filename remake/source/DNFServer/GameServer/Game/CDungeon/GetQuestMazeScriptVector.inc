std::vector<MazeScript>* CDungeon::GetQuestMazeScriptVector(
    ENUM_MAZE_QUEST_TYPE mazeQuestType) const
{
    return const_cast<std::vector<MazeScript>*>(&m_mazeList0[mazeQuestType]);
}