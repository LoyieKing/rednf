void CTitleBookNoClear::execute()
{
    reinterpret_cast<AchFlagView*>(GetUser()->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xf))->m_flag = 1;
