int Dispatcher_Float_RDATA_Modulation::process(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)param;
    // ORIG 0x?: `mov %eax,-0x18(%ebp); cmpl $0x0,-0x18(%ebp)` —— err 具名占槽
    // （我方原先内联 `check_error(...) != 0` 产 `test;setne;test` 三连，且挤掉 msg 引用槽号）。
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_FLOAT_RDATA_MODULATION& m = reinterpret_cast<MSG_FLOAT_RDATA_MODULATION&>(msg);
    unsigned int d1 = m.m_encoded1 ^ 0x59ab991f;
    unsigned int d2 = m.m_encoded2 ^ 0x1f93712b;
    WongWork::CHackAnalyzer* ha = user->getHackAnalyzer();
    ha->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x349, 1, d1, d2);
    return 0;
