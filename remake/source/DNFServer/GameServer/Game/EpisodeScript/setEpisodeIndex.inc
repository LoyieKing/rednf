void EpisodeScript::setEpisodeIndex(int index, int questIndex)
{
    m_episodeIndex = (index << 16) | questIndex;
}