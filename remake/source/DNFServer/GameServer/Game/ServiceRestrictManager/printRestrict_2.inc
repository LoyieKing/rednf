void CreateCharacterRestrictPolicy::printRestrict()                 // ORIG 0816df6c
{
    // b28c-L20: AE — 同 SecurityRestrictPolicy::printRestrict 修法（P221/P37 全表达式
    // 临时链翻正 4.4 的 cMyTrace 槽落位，ORIG trace1 -0x3c/trace2 -0x2c/trace3 -0x1c）；
    // b26「[N82] ndiff 12 封顶」判词作废。
    cMyTrace(__PRETTY_FUNCTION__, 0x125, 9, true, true)("\tCreate Character Begin");
    for (unsigned int i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x128, 9, true, true)(
            "\t- %s : %d",
            CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x12c, 9, true, true)("\tCreate Character End");
