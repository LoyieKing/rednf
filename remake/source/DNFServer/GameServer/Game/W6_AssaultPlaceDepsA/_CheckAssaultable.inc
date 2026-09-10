bool CAssaultPlace::_CheckAssaultable()
{
    int counts[2] = { 0, 0 };  // asm 仅清 -0x14/-0x10 两槽
    _GetTeamCount(counts, (ENUM_ASSAULT_STATE)5);
    for (int i = 0; i <= 1; ++i) {
        if (counts[i] == 0)
            return false;
    }
    return true;
