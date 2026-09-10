int Dispatcher_ScanBotByDrv::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x08264740
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_SCAN_DRV& req = reinterpret_cast<MSG_SCAN_DRV&>(msg);
    int scanResult = req.m_field0d;
    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
    analyzer->setBaseHackType((WongWork::ENUM_HACKTYPE)0x708);
    int setEtcRet = user->SetETC((short)(scanResult + 0x708), (int)(char)req.m_field11);
    analyzer = user->getHackAnalyzer();
    analyzer->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)(scanResult + 0x708),
                               1, (unsigned int)setEtcRet, 0);
    return 0;
