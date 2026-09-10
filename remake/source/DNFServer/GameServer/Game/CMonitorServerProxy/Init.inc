bool CMonitorServerProxy::Init()
{
    if (m_udpPort == 0)
    {
        return 1;
    }
    if (InitUdp() == false)
    {
        return 0;
    }
    if (InitTcp() == false)
    {
        return 0;
    }
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        GlobalData::gmList->SendGMList_To_UpperServer(0);
    }
    return 1;
