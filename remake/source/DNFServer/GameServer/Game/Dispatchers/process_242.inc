int Dispatcher_Float_RDATA_Modulation::process(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)param;
    if (check_error(user, msg) != 0)
    {
        return 0;
    }
    MSG_FLOAT_RDATA_MODULATION& m = reinterpret_cast<MSG_FLOAT_RDATA_MODULATION&>(msg);
    unsigned int d1 = m.m_encoded1 ^ 0x59ab991f;
    unsigned int d2 = m.m_encoded2 ^ 0x1f93712b;
    WongWork::CHackAnalyzer* ha = user->getHackAnalyzer();
    ha->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x349, 1, d1, d2);
    return 0;
