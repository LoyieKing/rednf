// ORIG _ZN12TutorialData16isCurrentEpisodeEjj @0x08ab84fe
bool TutorialData::isCurrentEpisode(unsigned int episodeNo, unsigned int count)
{
    if (episodeNo < getDialogSize(count)) {
        return true;
    }
    return false;
}