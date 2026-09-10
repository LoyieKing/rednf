void CHackAnalyzer::onUpdateHackCount(CUser* user, ENUM_HACKTYPE type, int a,
                                      unsigned int b, unsigned int c)
{
    user->onUpdateHackCount(type, a, b, c);
