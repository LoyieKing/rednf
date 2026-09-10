void AchievementRewardData::reset()
{
    m_type = 0;                    // +0x00
    m_difficulty = -1;             // +0x04
    m_stageClearCount = 0;         // +0x08
    m_iconImage = -1;              // +0x0c
    m_rewardItems.clear();         // +0x10
    m_rewardTitle.clear();         // +0x1c
    m_postalTitle.clear();         // +0x20
    m_postalLetter.clear();        // +0x24
