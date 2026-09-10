void cUserHistoryLog::BloodRoundClear(int round, unsigned int clearTime,
                                      unsigned int playTime,
                                      const char* dungeonName,
                                      const char* memberNames)
{
    (void)playTime;
    (void)dungeonName;
    // ORIG 0x08684af0：无 null 检查，直接解引用。
    m_user->LogHistory("BloodRoundClear,%d,%u,%s", (int)round,
                       (unsigned int)clearTime, memberNames);
