void CHackAnalyzer::_reportIncurrectReport()
{
    // ORIG W 0x080f8032（_reportIncurrectReport.md）。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_INCURRECT))
        return;
    _addHackInfo(REPORT_4_HACK_FLAG_INCURRECT, (ENUM_HACKTYPE)0x3c4,
                 ((CUser*)m_baseHackType)->get_state());
}