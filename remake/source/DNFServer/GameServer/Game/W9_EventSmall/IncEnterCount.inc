void CCharacBloodDungeon::IncEnterCount()            // ORIG 0x85bfea0 (W)
{
    charac_expand::CData::alter();
    ++m_enterCount;
    ++m_weeklyEnterCount;
