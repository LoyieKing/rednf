void AuthMobileRestrictPolicy::printRestrict()                      // ORIG 0816d866
{
    // b28c-L20: AE — 同 SecurityRestrictPolicy::printRestrict：三处 cMyTrace 改
    // 全表达式临时链（P221/P37）后 4.4 槽落位翻正（ORIG trace1 -0x3c/trace2 -0x2c/
    // trace3 -0x1c），b26「[N82] ndiff 12 封顶」判词作废；71=71 ndiff 0。
    unsigned int i;
    cMyTrace(__PRETTY_FUNCTION__, 0x8d, 9, true, true)("\tMobile Auth Begin");
    for (i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x90, 9, true, true)(
            "\t- %s : %d",
            MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x94, 9, true, true)("\tMobile Auth End");
}