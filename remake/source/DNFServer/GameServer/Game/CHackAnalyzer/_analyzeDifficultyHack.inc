void CHackAnalyzer::_analyzeDifficultyHack(char level)
{
    // ORIG 0x080f7d82（_analyzeDifficultyHack.md）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_DIFFICULTY))
        return;
    _addHackInfo(REPORT_4_HACK_FLAG_DIFFICULTY, (ENUM_HACKTYPE)0x3bf,
                 (int)level);
