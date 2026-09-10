bool CAssaultPlace::_CheckPlayable()
{
    // ORIG T 0x82e9142：两队人数均非 0 才可开局。
    int counts[2] = { 0, 0 };
    _GetTeamCount(counts);
    for (int i = 0; i < 2; ++i) {
        if (counts[i] == 0)
            return false;
    }
    return true;
