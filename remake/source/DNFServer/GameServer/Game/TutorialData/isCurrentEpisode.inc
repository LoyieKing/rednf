bool TutorialData::isCurrentEpisode(unsigned int episodeNo, unsigned int count)
{
    if (episodeNo < getDialogSize(count)) {
        return true;
    }
    return false;
